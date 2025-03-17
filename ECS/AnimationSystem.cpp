#include "AnimationSystem.h"

bool AnimationSystem::start()
{
    return true;
}

void AnimationSystem::update(int time)
{
    for (auto &entity : m_entities)
    {
        SpriteComponent *sprite = entity->getComponent<SpriteComponent>();
        if (sprite && sprite->isAnimated())
        {
            sprite->setClipIndex(time / sprite->getTimePerClip() % sprite->getNumClips());
        }
    }
}

void AnimationSystem::end()
{

}
