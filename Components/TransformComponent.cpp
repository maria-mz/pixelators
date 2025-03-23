#include "TransformComponent.h"

TransformComponent::TransformComponent()
{
    m_posX = 0;
    m_posY = 0;
    m_width = 0;
    m_height = 0;
}

TransformComponent::TransformComponent(int posX, int posY, int width, int height)
{
    m_posX = posX;
    m_posY = posY;
    m_width = width;
    m_height = height;
}

int TransformComponent::posX()
{
    return m_posX;
}

int TransformComponent::posY()
{
    return m_posY;
}

int TransformComponent::width()
{
    return m_width;
}

int TransformComponent::height()
{
    return m_height;
}

void TransformComponent::setPosX(int posX)
{
    m_posX = posX;
}

void TransformComponent::setPosY(int posY)
{
    m_posY = posY;
}

void TransformComponent::setWidth(int width)
{
    m_width = width;
}

void TransformComponent::setHeight(int height)
{
    m_height = height;
}

void TransformComponent::move(int dx, int dy)
{
    m_posX += dx;
    m_posY += dy;
}
