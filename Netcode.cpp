#include "Netcode.h"

void Netcode::updateNetState(GameMessage &updateMsg)
{
    m_netPlayerData.position.x = updateMsg.posX;
    m_netPlayerData.position.y = updateMsg.posY;
    m_netPlayerData.velocity.x = updateMsg.velX;
    m_netPlayerData.velocity.y = updateMsg.velY;
}

void Netcode::setNetPlayerData(NetPlayerData data)
{
    m_netPlayerData.position.x = data.position.x;
    m_netPlayerData.position.y = data.position.y;
    m_netPlayerData.velocity.x = data.velocity.x;
    m_netPlayerData.velocity.y = data.velocity.y;
}

bool Netcode::isPlayerDesynced(Player &player)
{
    float dx = player.m_position->x - m_netPlayerData.position.x;
    float dy = player.m_position->y - m_netPlayerData.position.y;

    const int maxPositionDelta = 2;

    return (abs(dx) > maxPositionDelta || abs(dy) > maxPositionDelta);
}

void Netcode::syncPlayerWithNetState(Player &player)
{
    if (!isPlayerDesynced(player))
    {
        return;
    }

    printf("Correcting opponent!\n");
    printf("======== Debug ========\n");

    printf(
        "Pos: predicted (%.6f, %.6f) | net (%.6f, %.6f)\n",
        player.m_position->x, player.m_position->y,
        m_netPlayerData.position.x, m_netPlayerData.position.y
    );

    printf(
        "Vel: predicted (%.6f, %.6f) | net (%.6f, %.6f)\n",
        player.m_velocity->x, player.m_velocity->y,
        m_netPlayerData.velocity.x, m_netPlayerData.velocity.y
    );

    float dx = player.m_position->x - m_netPlayerData.position.x;
    float dy = player.m_position->y - m_netPlayerData.position.y;
    float dvx = player.m_velocity->x - m_netPlayerData.velocity.x;
    float dvy = player.m_velocity->y - m_netPlayerData.velocity.y;

    printf("ΔPos: (%.6f, %.6f) | ΔVel: (%.6f, %.6f)\n", dx, dy, dvx, dvy);

    player.setVelocity(m_netPlayerData.velocity.x, m_netPlayerData.velocity.y);

    const float lerpFactor = 0.3f;

    float lerpedX = std::lerp(player.m_position->x, m_netPlayerData.position.x, lerpFactor);
    float lerpedY = std::lerp(player.m_position->y, m_netPlayerData.position.y, lerpFactor);

    printf("New Pos (lerped): (%.6f, %.6f)\n", lerpedX, lerpedY);
    printf("=======================\n\n");

    player.m_position->x = lerpedX;
    player.m_position->y = lerpedY;
}
