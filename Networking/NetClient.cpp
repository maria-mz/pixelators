#include "NetClient.h"


NetClient::~NetClient()
{
    disconnect();
}


bool NetClient::connectToServer(std::string host, std::string port)
{
    bool success = true;

    printf("[CLIENT] Connecting to server at %s:%s\n", host.c_str(), port.c_str());

    try
    {
        m_connection = NetConnection::create(m_ioContext);

        asio::ip::tcp::resolver resolver(m_ioContext);
        asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, port);

        asio::connect(m_connection->m_socket, endpoints);
    }
    catch (std::exception &e)
    {
        success = false;
        printf("[CLIENT] Failed to connect to server: %s\n", e.what());

        if (m_connection->m_socket.is_open())
        {
            m_connection->m_socket.close();
        }
    }

    if (success)
    {
        m_connection->startReadLoop();

        m_contextThread = std::thread(
            [this]()
            {
                try
                {
                    printf("[CLIENT] ASIO context starting\n");
                    m_ioContext.run();
                    printf("[CLIENT] ASIO context stopped\n");
                }
                catch (const std::system_error& e)
                {
                    // Socket already closed
                }
            }
        );
    }

    return success;
}


bool NetClient::isConnected()
{
    return (m_connection && m_connection->m_socket.is_open());
}


void NetClient::disconnect()
{
    if (!m_ioContext.stopped())
    {
        m_ioContext.stop();
    }

    // Using try-catch here because even if we check if the socket is open first,
    // by the time we call close an async read or write in the ASIO context thread
    // may have already called close -> would throw error anyway
    try
    {
        if (m_connection)
        {
            m_connection->m_socket.close();
        }
    }
    catch(const std::system_error& e)
    {
        // Socket already closed
    }

    if (m_contextThread.joinable())
    {
        // This hangs sometimes... whyy??
        printf("[CLIENT] Joining ASIO context thread\n");
        m_contextThread.join();
    }
}


bool NetClient::send(NetMessage &msg)
{
    bool ok = false;

    if (m_connection)
    {
        m_connection->send(msg);
        ok = true;
    }

    return ok;
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
