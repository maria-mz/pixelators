#include "BattleScene.h"

BattleScene::BattleScene()
{
    m_player = nullptr;

    m_leftWall = new Wall();
    m_rightWall = new Wall();

    m_leftWall->setTransformComponent(new TransformComponent(0, 0, 0, PX_WINDOW_HEIGHT));
    m_rightWall->setTransformComponent(new TransformComponent(PX_WINDOW_WIDTH, 0, 0, PX_WINDOW_HEIGHT));
}

BattleScene::~BattleScene()
{
    if (m_player)
    {
        delete m_player;
    }

    delete m_leftWall;
    delete m_rightWall;
}

void BattleScene::setPlayer(Player *player)
{
    m_player = player;
}

Player* BattleScene::getPlayer()
{
    return m_player;
}

Wall* BattleScene::getLeftWall()
{
    return m_leftWall;
}

Wall* BattleScene::getRightWall()
{
    return m_rightWall;
}
