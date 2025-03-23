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
    TransformComponent *transform = m_scene->getPlayer()->transformComponent();
    VelocityComponent *velocity = m_scene->getPlayer()->velocityComponent();

    transform->setPosX(transform->posX() + velocity->velX());
}

void MoveSystem::end()
{
    // Nothing to do yet
}
