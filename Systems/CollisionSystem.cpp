#include "CollisionSystem.h"

bool CollisionSystem::start()
{
    return true;
}

void CollisionSystem::input(SDL_Event &e)
{

}

void CollisionSystem::update(int time)
{
    TransformComponent *tformPlayer = m_scene->getPlayer()->transformComponent();
    TransformComponent *tformLeftWall = m_scene->getLeftWall()->transformComponent();
    TransformComponent *tformRightWall = m_scene->getRightWall()->transformComponent();

    if (tformLeftWall->posX() + tformLeftWall->width() >= tformPlayer->posX())
    {
        tformPlayer->move(-(tformPlayer->posX() - tformLeftWall->posX() - tformLeftWall->width()), 0);
    }
    else if (tformPlayer->posX() + tformPlayer->width() >= tformRightWall->posX())
    {
        tformPlayer->move(-(tformPlayer->posX() - tformRightWall->posX() + tformPlayer->width()), 0);
    }
}

void CollisionSystem::end()
{

}
