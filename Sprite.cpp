#include "Sprite.h"

SpriteClipper makeSpriteClipper(int originalWidth, int originalHeight, int scale)
{
    return [originalWidth, originalHeight, scale](int pixelsFromLeft,
                                                  int pixelsFromTop,
                                                  int entityWidth,
                                                  int entityHeight,
                                                  int row,
                                                  int col)
    {
        SDL_Rect r;
        r.x = (pixelsFromLeft * scale) + (col * (originalWidth * scale));
        r.y = (pixelsFromTop * scale) + (row * (originalHeight * scale));
        r.w = entityWidth * scale;
        r.h = entityHeight * scale;

        return r;
    };
}

Animation::Animation(int fps)
{
    m_timePerFrame = SECONDS_IN_MS / fps;
    m_frameIndex = 0;
    m_accumulatedTime = 0;
    m_flip = SDL_FLIP_NONE;
}

void Animation::setFrames(std::vector<SDL_Rect> &frames)
{
    m_frames = frames;
}

void Animation::setFlip(SDL_RendererFlip flip)
{
    m_flip = flip;
}

SDL_Rect Animation::getCurrentFrame()
{
    return m_frames[m_frameIndex];
}

SDL_RendererFlip Animation::getFlip()
{
    return m_flip;
}

void Animation::update(int deltaTime)
{
    m_accumulatedTime += deltaTime;

    while (m_accumulatedTime >= m_timePerFrame)
    {
        m_frameIndex++;
        if (m_frameIndex >= m_frames.size())
        {
            m_frameIndex = 0;
        }
        m_accumulatedTime -= m_timePerFrame;
    }
}

void Animation::reset()
{
    m_frameIndex = 0;
}


Animator::Animator()
{
    m_currentAnimationTag = -1;
}

Animator::~Animator()
{
    for (auto& pair : m_animations) {
        delete pair.second;
    }
}

void Animator::addAnimation(int animationTag, Animation *animation)
{
    m_animations[animationTag] = animation;
}

Animation *Animator::getCurrentAnimation()
{
    return m_animations[m_currentAnimationTag];
}

void Animator::setCurrentAnimation(int animationTag)
{
    Animation *currentAnimation = getCurrentAnimation();
    if (currentAnimation)
    {
        currentAnimation->reset();
    }

    m_currentAnimationTag = animationTag;
}

void Animator::updateCurrentAnimation(int deltaTime)
{
    Animation *currentAnimation = getCurrentAnimation();

    if (currentAnimation)
    {
        currentAnimation->update(deltaTime);
    }
}

Animation *Animator::getAnimation(int animationTag)
{
    return m_animations[animationTag];
}


Sprite::Sprite()
{
    m_animator = nullptr;
}

Sprite::~Sprite()
{
    delete m_animator;
}

void Sprite::setAnimator(Animator *animator)
{
    m_animator = animator;
}

Animator *Sprite::getAnimator()
{
    return m_animator;
}
