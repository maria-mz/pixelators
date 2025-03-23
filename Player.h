#ifndef PLAYER_H_
#define PLAYER_H_

#include "Components/Components.h"
#include "SDL2/SDL.h"

enum class PlayerState
{
    Idle,
    Running
};

class Player
{
    public:
        Player();
        ~Player();

        void setTransformComponent(TransformComponent* transform);
        void setVelocityComponent(VelocityComponent* velocity);
        void setSpriteComponent(SpriteComponent* sprite);

        TransformComponent* transformComponent();
        VelocityComponent* velocityComponent();
        SpriteComponent* spriteComponent();

        PlayerState currentState();
        void setCurrentState(PlayerState state);

    private:
        TransformComponent* m_transform;
        VelocityComponent* m_velocity;
        SpriteComponent* m_sprite;

        PlayerState m_currentState;
};

#endif