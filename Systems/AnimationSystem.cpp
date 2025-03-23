#include "AnimationSystem.h"

bool AnimationSystem::start()
{
    return true;
}

void AnimationSystem::input(SDL_Event &e)
{

}

void AnimationSystem::update(int time)
{
    SpriteComponent *sprite = m_player->spriteComponent();
    if (sprite)
    {
        Animation *animation = sprite->getCurrentAnimation();
        animation->setClipIndex(time / animation->getTimePerClip() % animation->getNumClips());
    }
}

void AnimationSystem::end()
{

}
