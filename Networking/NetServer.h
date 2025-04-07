#ifndef NET_SERVER_H_
#define NET_SERVER_H_

#include <asio.hpp>
#include <unordered_map>
#include <set>
#include <mutex>

#include "NetConnection.h"
#include "NetMessages.h"


using ClientID = u_int32_t;


class NetServer
{
    public:
        NetServer(int port)
        : m_acceptor(m_ioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {}
        ~NetServer();

        void start();
        void shutdown();

        bool send(ClientID clientID, NetMessage &msg);
        bool recv(ClientID clientID, NetMessage &msg);
        bool blockingRecv(ClientID clientID, NetMessage &msg);

        bool isClientConnected(ClientID clientID);
        void disconnectClient(ClientID clientID);

    private:
        void accept();

        asio::io_context m_ioContext;
        std::thread m_contextThread;

        asio::ip::tcp::acceptor m_acceptor;

        std::unordered_map<ClientID, std::shared_ptr<NetConnection>> m_clients;
        std::shared_ptr<NetConnection> getClient(ClientID clientID);

        ClientID m_nextNewClientID = 10000;
        ClientID getNewClientID();
};


NetServer::~NetServer()
{
    shutdown();
}


void NetServer::accept()
{
    std::shared_ptr<NetConnection> newClient = NetConnection::create(m_ioContext);

    m_acceptor.async_accept(
        newClient->m_socket,
        [this, newClient](const std::error_code &ec)
        {
            if (!ec)
            {
                ClientID clientID = getNewClientID();
                newClient->m_id = clientID;

                printf("[SERVER] Accepting new client with ID %d\n", clientID);
                m_clients[clientID] = newClient;

                newClient->startReadLoop();

                NetMessage msg{NetMessageType::ConnectOk, ConnectOkBody{clientID}};
                newClient->send(msg);
            }

            accept();
        }
    );
}


ClientID NetServer::getNewClientID()
{
    return m_nextNewClientID++;
}


void NetServer::disconnectClient(ClientID clientID)
{
    std::shared_ptr<NetConnection> client = getClient(clientID);

    if (client && client->m_socket.is_open())
    {
        try
        {
            client->m_socket.close();
        }
        catch(const std::system_error& e)
        {
            // Socket already closed
        }

        m_clients.erase(clientID);

        printf("[SERVER] Disconnected client %d\n", clientID);
    }
}


void NetServer::start()
{
    accept();
    m_contextThread = std::thread(
        [this]()
        {
            try
            {
                m_ioContext.run();
            }
            catch(const std::system_error& e)
            {
                // Socket already closed
            }
        }
    );
}


void NetServer::shutdown()
{
    m_ioContext.stop();

    std::error_code ec;
    m_acceptor.cancel(ec);
    m_acceptor.close(ec);

    std::vector<ClientID> clientIDs;

    for (auto &pair : m_clients)
    {
        clientIDs.push_back(pair.first);
    }

    for (auto clientID : clientIDs)
    {
        disconnectClient(clientID);
    }

    if (m_contextThread.joinable())
    {
        printf("[SERVER] Joining ASIO context thread\n");
        m_contextThread.join();
    }
}


bool NetServer::send(ClientID clientID, NetMessage &msg)
{
    bool ok = false;

    std::shared_ptr<NetConnection> client = getClient(clientID);

    if (client)
    {
        client->send(msg);
        ok = true;
    }

    return ok;
}


bool NetServer::recv(ClientID clientID, NetMessage &msg)
{
    bool ok = false;

    std::shared_ptr<NetConnection> client = getClient(clientID);

    if (client)
    {
        ok = client->recv(msg);
    }

    return ok;
}


bool NetServer::blockingRecv(ClientID clientID, NetMessage &msg)
{
    bool ok = false;

    std::shared_ptr<NetConnection> client = getClient(clientID);

    if (client)
    {
        ok = client->blockingRecv(msg);
    }

    return ok;
}


std::shared_ptr<NetConnection> NetServer::getClient(ClientID clientID)
{
    auto it = m_clients.find(clientID);

    if (it != m_clients.end())
    {
        return it->second;
    }

    return nullptr;
}


bool NetServer::isClientConnected(ClientID clientID)
{
    std::shared_ptr<NetConnection> client = getClient(clientID);

    if (client && client->m_socket.is_open())
    {
        return true;
    }
    else
    {
        return false;
    }
}


#endif