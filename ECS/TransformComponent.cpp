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

int TransformComponent::getPosX()
{
    return m_posX;
}

int TransformComponent::getPosY()
{
    return m_posY;
}

int TransformComponent::getWidth()
{
    return m_width;
}

int TransformComponent::getHeight()
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
