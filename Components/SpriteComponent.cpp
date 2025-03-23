#include "SpriteComponent.h"

Animation::Animation(int animationFPS)
{
    m_texture = nullptr;

    m_timePerClip = SECONDS_IN_MS / animationFPS;
    m_clipIndex = 0;
}

void Animation::setTexture(SDL_Texture* texture)
{
    m_texture = texture;
}

SDL_Texture* Animation::getTexture()
{
    return m_texture;
}

void Animation::addClip(SDL_Rect spriteClip)
{

    m_spriteClips.push_back(spriteClip);
}

SDL_Rect Animation::getClipAtIndex(int index)
{
    return m_spriteClips[index];
}

SDL_Rect Animation::getCurrentClip()
{
    return getClipAtIndex(m_clipIndex);
}

int Animation::getTimePerClip()
{
    return m_timePerClip;
}

int Animation::getNumClips()
{
    return m_spriteClips.size();
}

void Animation::setClipIndex(int index)
{
    m_clipIndex = index;
}

void Animation::reset()
{
    m_clipIndex = 0;
}


SpriteComponent::SpriteComponent()
{
    m_currentTag = "";
}

SpriteComponent::~SpriteComponent()
{
    for (auto& pair : m_animations) {
        printf("Freeing Animation %s\n", pair.first.c_str());
        delete pair.second;
    }
}

void SpriteComponent::addAnimation(std::string tag, Animation *animation)
{
    m_animations[tag] = animation;
}

Animation* SpriteComponent::getCurrentAnimation()
{
    return m_animations[m_currentTag];
}

void SpriteComponent::setCurrentAnimation(std::string tag)
{
    Animation *currentAnimation = getCurrentAnimation();
    if (currentAnimation)
    {
        currentAnimation->reset();
    }

    m_currentTag = tag;
}
