#include "NetServer.h"


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

    if (client)
    {
        // Using try-catch here because even if we check if the socket is open first,
        // by the time we call close an async read or write in the ASIO context thread
        // may have already called close -> would throw error anyway
        try
        {
            client->m_socket.close();
        }
        catch(const std::system_error& e)
        {
            // Socket already closed
        }
    }
}


void NetServer::removeClient(ClientID clientID)
{
    disconnectClient(clientID);
    m_clients.erase(clientID);
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
        disconnectClient(pair.first);
    }

    m_clients.clear();

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


void NetServer::broadcast(NetMessage &msg)
{
    for (auto &pair : m_clients)
    {
        pair.second->send(msg);
    }
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
