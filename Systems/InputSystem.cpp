#include "InputSystem.h"

bool InputSystem::start()
{
    return true;
}

void InputSystem::input(SDL_Event &e)
{
    VelocityComponent *velocity = m_player->velocityComponent();
    SpriteComponent *sprite = m_player->spriteComponent();

    if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
    {
        switch (e.key.keysym.sym)
        {
            case SDLK_d:
                velocity->setVelX(velocity->velX() + velocity->maxVelocity());
                break;
            case SDLK_a:
                velocity->setVelX(velocity->velX() - velocity->maxVelocity());
                break;
        }
    }
    if (e.type == SDL_KEYUP && e.key.repeat == 0)
    {
        switch (e.key.keysym.sym)
        {
            case SDLK_d:
                velocity->setVelX(velocity->velX() - velocity->maxVelocity());
                break;
            case SDLK_a:
                velocity->setVelX(velocity->velX() + velocity->maxVelocity());
                break;
        }
    }
    if (velocity->velX() == 0)
    {
        m_player->setCurrentState(PlayerState::Idle);
        sprite->setCurrentAnimation("idle");
    }
    else
    {
        m_player->setCurrentState(PlayerState::Running);
        sprite->setCurrentAnimation("running");
    }
}

void InputSystem::update(int time)
{
    // Nothing to update yet
}

void InputSystem::end()
{
    // Nothing to do yet
}
