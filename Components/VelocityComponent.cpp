#include "VelocityComponent.h"

VelocityComponent::VelocityComponent()
{
    m_velX = 0;
    m_velY = 0;
    m_maxVelocity = 0;
}

VelocityComponent::VelocityComponent(int velX, int velY, int maxVelocity)
{
    m_velX = velX;
    m_velY = velY;
    m_maxVelocity = maxVelocity;
}

int VelocityComponent::velX()
{
    return m_velX;
}

int VelocityComponent::velY()
{
    return m_velY;
}

int VelocityComponent::maxVelocity()
{
    return m_maxVelocity;
}

void VelocityComponent::setVelX(int velX)
{
    m_velX = velX;
}

void VelocityComponent::setVelY(int velY)
{
    m_velY = velY;
}
