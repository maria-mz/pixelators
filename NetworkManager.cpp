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

bool NetworkManager::connectToServer()
{
    if (m_isHost)
    {
        throw std::logic_error("A host cannot connect to the server (it is the server)");
    }
    else
    {
        // hardcoded host for now
        return m_client->connectToServer("127.0.0.1", std::to_string(SERVER_PORT));
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

void NetworkManager::sendPlayerInputEvent(Event event)
{
    NetMessage msg{NetMessageType::Data, GameMessage{event}};

    if (m_isHost)
    {
        m_server->broadcast(msg);
    }
    else
    {
        m_client->send(msg);
    }
}

Event NetworkManager::receiveOpponentInputEvent()
{
    NetMessage msg;

    if (m_isHost)
    {
        // uh.. opponent client id hardcoded for now
        m_server->recv(10000, msg);
    }
    else
    {
        m_client->recv(msg);
    }

    if (msg.header.type == NetMessageType::Data)
    {
        return msg.body<GameMessage>().event;
    }

    return EVENT_NONE;
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
