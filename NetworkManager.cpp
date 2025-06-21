#include "NetworkManager.h"

void NetworkManager::init()
{
    if (m_isHost)
    {
        m_server = std::unique_ptr<NetServer>(new NetServer(SERVER_PORT));
        m_server->start();

        m_server->setOnClientConnect([this](ClientID clientID) {
            m_opponentClientID = clientID;
        });
    }
    else
    {
        m_client = std::unique_ptr<NetClient>(new NetClient());
    }
}

bool NetworkManager::connectToServer(int maxAttempts, int waitBetweenAttempts_ms)
{
    if (m_isHost)
    {
        throw std::logic_error("A host cannot connect to the server (it is the server)");
    }
    else
    {
        int attempts = 0;
        while (attempts < maxAttempts && !isConnectedToServer())
        {
            LOG_INFO("Trying to connect to server... (Attempt %d)", attempts + 1);
            m_client->connectToServer(SERVER_HOST, std::to_string(SERVER_PORT));
            attempts++;

            std::this_thread::sleep_for(std::chrono::milliseconds(waitBetweenAttempts_ms));
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
        m_server->recv(m_opponentClientID, netMsg);
    }
    else
    {
        m_client->recv(netMsg);
    }

    if (netMsg.header.type == NetMessageType::Data)
    {
        msg = netMsg.body<GameMessage>();
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
