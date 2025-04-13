#include "NetworkManager.h"

void NetworkManager::init()
{
    if (m_isHost)
    {
        m_server = std::unique_ptr<NetServer>(new NetServer(SERVER_PORT));
        m_server->start();
    }
    else
    {
        m_client = std::unique_ptr<NetClient>(new NetClient());
    }
}

bool NetworkManager::connectToServer(int maxRetries, int waitBetweenRetriesMs)
{
    if (m_isHost)
    {
        throw std::logic_error("A host cannot connect to the server (it is the server)");
    }
    else
    {
        int retries = 0;
        while (retries < maxRetries && !isConnectedToServer())
        {
            printf("Trying to connect to server...\n");
            m_client->connectToServer(SERVER_HOST, std::to_string(SERVER_PORT));
            retries++;

            std::this_thread::sleep_for(std::chrono::milliseconds(waitBetweenRetriesMs));
        }

        return isConnectedToServer();
    }
}

bool NetworkManager::isConnectedToServer()
{
    if (m_isHost)
    {
        throw std::logic_error("A host cannot be connected to the server (it is the server)");
    }
    else
    {
        return m_client->isConnected();
    }
}

void NetworkManager::sendPlayerMsg(GameMessage &msg)
{
    NetMessage netMsg{NetMessageType::Data, msg};

    // printf("Sending PLAYER State. msg.posX=%f, msg.posY=%f, msg.state=%d\n", msg.posX, msg.posY, msg.state);

    if (m_isHost)
    {
        m_server->broadcast(netMsg);
    }
    else
    {
        m_client->send(netMsg);
    }
}

bool NetworkManager::receiveOpponentMsg(GameMessage &msg)
{
    NetMessage netMsg;
    bool ok = false;

    if (m_isHost)
    {
        // uh.. opponent client id hardcoded for now
        m_server->recv(10000, netMsg);
    }
    else
    {
        m_client->recv(netMsg);
    }

    if (netMsg.header.type == NetMessageType::Disconnect)
    {
        printf("Disconnected!\n");
    }

    if (netMsg.header.type == NetMessageType::Data)
    {
        msg = netMsg.body<GameMessage>();
        // printf("Received OPPONENT State. msg.posX=%f, msg.posY=%f, msg.state=%d\n", msg.posX, msg.posY, msg.state);
        ok = true;
    }

    return ok;
}

void NetworkManager::shutdown()
{
    if (m_server)
    {
        m_server->shutdown();
    }
    if (m_client)
    {
        m_client->disconnect();
    }
}
