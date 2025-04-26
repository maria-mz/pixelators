#include "Sprite.h"

SDL_Rect createClipFromSpriteSheet(
    int spriteWidth,
    int spriteHeight,
    int pixelsFromLeft,
    int pixelsFromTop,
    int entityWidth,
    int entityHeight,
    int row,
    int col
)
{
    SDL_Rect r;
    r.x = pixelsFromLeft + (col * spriteWidth);
    r.y = pixelsFromTop + (row * spriteHeight);
    r.w = entityWidth;
    r.h = entityHeight;

    return r;
}

SDL_Rect scaleRect(SDL_Rect r, int scale)
{
    r.x *= scale;
    r.y *= scale;
    r.w *= scale;
    r.h *= scale;

    return r;
}

Animation::Animation(int fps, bool repeats)
{
    m_repeats = repeats;
    m_timePerFrame = SECONDS_IN_MS / fps;
    m_frameIndex = 0;
    m_accumulatedTime = 0;
    m_flip = SDL_FLIP_NONE;
}

void Animation::setFrames(std::vector<Frame> &frames)
{
    m_frames = frames;
}

void Animation::setFlip(SDL_RendererFlip flip)
{
    m_flip = flip;

    for (auto &frame : m_frames)
    {
        frame.setFlip(flip);
    }
}

Frame Animation::getCurrentFrame()
{
    return m_frames[m_frameIndex];
}

SDL_RendererFlip Animation::getFlip()
{
    return m_flip;
}

bool Animation::isDone()
{
    return m_isDone;
}

void Animation::update(int deltaTime)
{
    if (m_isDone)
    {
        return;
    }

    m_accumulatedTime += deltaTime;

    while (m_accumulatedTime >= m_timePerFrame)
    {
        m_frameIndex++;
        if (m_frameIndex >= m_frames.size())
        {
            if (!m_repeats)
            {
                m_isDone = true;
                m_frameIndex--; // Stop at last frame
            }
            else
            {
                m_frameIndex = 0;
            }
        }
        m_accumulatedTime -= m_timePerFrame;
    }
}

void Animation::reset()
{
    m_frameIndex = 0;
    m_isDone = false;
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
