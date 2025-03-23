#include "MoveSystem.h"

bool MoveSystem::start()
{
    return true;
}

void MoveSystem::input(SDL_Event &e)
{

}

void MoveSystem::update(int time)
{
    TransformComponent *transform = m_player->transformComponent();
    VelocityComponent *velocity = m_player->velocityComponent();
    transform->setPosX(transform->posX() + velocity->velX());
}

void MoveSystem::end()
{
    // Nothing to do yet
}
