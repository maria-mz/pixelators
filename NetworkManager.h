#ifndef NETWORK_MANAGER_H_
#define NETWORK_MANAGER_H_

#include <thread>
#include <functional>

#include "Networking/NetClient.h"
#include "Networking/NetServer.h"
#include "Networking/NetMessages.h"
#include "Player.h"

// TODO: Hardcoded for now
constexpr const char *SERVER_HOST = "127.0.0.1";
constexpr int SERVER_PORT = 8080;

struct GameMessage
{
    float posX;
    float posY;
    float velX;
    float velY;
    PlayerStateName state;
};

class NetworkManager
{
    public:
        explicit NetworkManager(bool isHost) : m_isHost(isHost) {}

        void init();

        bool connectToServer(int maxRetries = 3, int waitBetweenRetriesMs = 1000);
        bool isConnectedToServer();

        void setOnOpponentMsg(std::function<void(GameMessage &msg)> onOpponentMsg);

        void start();
        void shutdown();

        void sendPlayerMsg(GameMessage &msg);

    private:
        bool receiveOpponentMsg(GameMessage &msg);

        std::unique_ptr<NetClient> m_client;
        std::unique_ptr<NetServer> m_server;

        bool m_isHost;

        std::thread m_receiveThread;
        bool m_isRunning;

        std::function<void(GameMessage &msg)> m_onOpponentMsgCallback = [](GameMessage &msg){};
};

#endif