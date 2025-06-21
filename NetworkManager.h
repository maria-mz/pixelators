#ifndef NETWORK_MANAGER_H_
#define NETWORK_MANAGER_H_

#include <thread>
#include <functional>

#include "Logging.h"
#include "Networking/NetClient.h"
#include "Networking/NetServer.h"
#include "Networking/NetMessages.h"
#include "Player.h"

#include "Input.h"

// TODO: Hardcoded for now
constexpr const char *SERVER_HOST = "127.0.0.1";
constexpr int SERVER_PORT = 8080;

struct GameMessage
{
    float posX;
    float posY;
    float velX;
    float velY;
    InputEvent inputEvent;
};

class NetworkManager
{
    public:
        explicit NetworkManager(bool isHost) : m_isHost(isHost) {}

        void init();

        bool connectToServer(int maxAttempts = 3, int waitBetweenAttempts_ms = 1000);
        bool isConnectedToServer();

        void shutdown();

        void sendPlayerMsg(GameMessage &msg);
        bool receiveOpponentMsg(GameMessage &msg);

    private:
        std::unique_ptr<NetClient> m_client;
        std::unique_ptr<NetServer> m_server;

        bool m_isHost;

        // Details relevant for server:
        int m_opponentClientID;
};

#endif