#ifndef NETCODE_H_
#define NETCODE_H_

#include <cmath>

#include "Player.h"
#include "NetworkManager.h"

struct NetPlayerData
{
    Vector2D<float> position;
    Vector2D<float> velocity;
};

class Netcode
{
    public:
        void updateNetState(GameMessage &updateMsg);

        bool isPlayerDesynced(Player &player);
        void syncPlayerWithNetState(Player &player);

        void setNetPlayerData(NetPlayerData data);

    private:
        NetPlayerData m_netPlayerData;
};

#endif