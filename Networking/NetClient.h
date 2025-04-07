#ifndef NET_CLIENT_H_
#define NET_CLIENT_H_

#include <mutex>
#include <thread>

#include <asio.hpp>

#include "NetConnection.h"
#include "NetMessages.h"


class NetClient
{
    public:
        ~NetClient();

        void connectToServer(std::string host, std::string port);
        bool isConnected();
        void disconnect();

        void send(NetMessage &msg);
        bool recv(NetMessage &msg);
        bool blockingRecv(NetMessage &msg);

    private:
        asio::io_context m_ioContext;
        std::thread m_contextThread;

        std::shared_ptr<NetConnection> m_connection;
};


NetClient::~NetClient()
{
    disconnect();
}


void NetClient::connectToServer(std::string host, std::string port)
{
    printf("[CLIENT] Connecting to server at %s:%s\n", host.c_str(), port.c_str());

    try
    {
        m_connection = NetConnection::create(m_ioContext);

        asio::ip::tcp::resolver resolver(m_ioContext);
        asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, port);

        asio::connect(m_connection->m_socket, endpoints);

        m_connection->startReadLoop();

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
    catch (std::exception &e)
    {
        printf("[CLIENT] Failed to connect to server: %s", e.what());
    }
}


bool NetClient::isConnected()
{
    return m_connection->m_socket.is_open();
}


void NetClient::disconnect()
{
    m_ioContext.stop();

    if (m_connection && m_connection->m_socket.is_open())
    {
        try
        {
            m_connection->m_socket.close();
        }
        catch(const std::system_error& e)
        {
            // Socket already closed
        }

        printf("[CLIENT] Disconnected from server\n");
    }

    if (m_contextThread.joinable())
    {
        // This hangs sometimes... whyy??
        printf("[CLIENT] Joining ASIO context thread\n");
        m_contextThread.join();
    }
}


void NetClient::send(NetMessage &msg)
{
    if (m_connection)
    {
        m_connection->send(msg);
    }
}


bool NetClient::recv(NetMessage &msg)
{
    bool ok = false;

    if (m_connection)
    {
        ok = m_connection->recv(msg);
    }

    return ok;
}


bool NetClient::blockingRecv(NetMessage &msg)
{
    bool ok = false;

    if (m_connection)
    {
        ok = m_connection->blockingRecv(msg);
    }

    return ok;
}


#endif