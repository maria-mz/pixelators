#include "SpriteComponent.h"

SpriteComponent::SpriteComponent()
{
    m_texture = nullptr;

    m_timePerClip = 0;
    m_clipIndex = 0;
}

SpriteComponent::SpriteComponent(int animationFPS)
{
    m_texture = nullptr;

    m_timePerClip = SECONDS_IN_MS / animationFPS;
    m_clipIndex = 0;
}

void SpriteComponent::setTexture(SDL_Texture* texture)
{
    m_texture = texture;
}

SDL_Texture* SpriteComponent::getTexture()
{
    return m_texture;
}

void SpriteComponent::addClip(SDL_Rect spriteClip)
{

    m_spriteClips.push_back(spriteClip);
}

SDL_Rect SpriteComponent::getClipAtIndex(int index)
{
    return m_spriteClips[index];
}

SDL_Rect SpriteComponent::getCurrentClip()
{
    return getClipAtIndex(m_clipIndex);
}

bool SpriteComponent::isAnimated()
{
    return m_timePerClip > 0;
}

int SpriteComponent::getTimePerClip()
{
    return m_timePerClip;
}

int SpriteComponent::getNumClips()
{
    return m_spriteClips.size();
}

void SpriteComponent::setClipIndex(int index)
{
    m_clipIndex = index;
}
