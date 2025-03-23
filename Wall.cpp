#include "Wall.h"

Wall::Wall()
{
    m_transform = nullptr;
}

Wall::~Wall()
{
    if (m_transform)
    {
        delete m_transform;
    }
}

void Wall::setTransformComponent(TransformComponent* transform)
{
    m_transform = transform;
}

TransformComponent* Wall::transformComponent()
{
    return m_transform;
}
