#ifndef NETWORK_MANAGER_H_
#define NETWORK_MANAGER_H_

#include "Networking/NetClient.h"
#include "Networking/NetServer.h"
#include "Networking/NetMessages.h"

#include "Event.h"

constexpr int SERVER_PORT = 8080;

struct GameMessage
{
    Event event;
};

class NetworkManager
{
    public:
        explicit NetworkManager(bool isHost) : m_isHost(isHost) {}

        void init();
        void shutdown();

        bool connectToServer();
        bool isConnectedToServer();

        void sendPlayerInputEvent(Event event);
        Event receiveOpponentInputEvent();

    private:
        std::unique_ptr<NetClient> m_client;
        std::unique_ptr<NetServer> m_server;

        bool m_isHost;
};

#endif