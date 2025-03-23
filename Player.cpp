#include "Player.h"

Player::Player()
{
    m_transform = nullptr;
    m_velocity = nullptr;
    m_sprite = nullptr;
    m_currentState = PlayerState::Idle;
}

Player::~Player()
{
    if (m_transform) {
        delete m_transform;
    }

    if (m_velocity) {
        delete m_velocity;
    }

    if (m_sprite) {
        delete m_sprite;
    }
}

void Player::setTransformComponent(TransformComponent* transform)
{
    m_transform = transform;
}

void Player::setVelocityComponent(VelocityComponent* velocity)
{
    m_velocity = velocity;
}

void Player::setSpriteComponent(SpriteComponent* sprite)
{
    m_sprite = sprite;
}

TransformComponent* Player::transformComponent()
{
    return m_transform;
}

VelocityComponent* Player::velocityComponent()
{
    return m_velocity;
}

SpriteComponent* Player::spriteComponent()
{
    return m_sprite;
}

PlayerState Player::currentState()
{
    return m_currentState;
}

void Player::setCurrentState(PlayerState state)
{
    m_currentState = state;
}