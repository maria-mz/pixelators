#ifndef SPRITE_H_
#define SPRITE_H_

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include <cassert>

#include "SDL2/SDL.h"

struct Frame
{
    int entityWidth;
    int entityHeight;
    SDL_Rect entityBoundingBox;
    SDL_Rect entityHitBox;
    SDL_Rect entityHurtBox;

    SDL_Rect textureClip;
    SDL_RendererFlip flip = SDL_FLIP_NONE;

    void setFlip(SDL_RendererFlip newFlip)
    {
        if (
            flip == SDL_FLIP_NONE && newFlip == SDL_FLIP_HORIZONTAL ||
            flip == SDL_FLIP_HORIZONTAL && newFlip == SDL_FLIP_NONE
        )
        {
            entityBoundingBox.x = entityWidth - entityBoundingBox.x - entityBoundingBox.w;
            entityHitBox.x = entityWidth - entityHitBox.x - entityHitBox.w;
            entityHurtBox.x = entityWidth - entityHurtBox.x - entityHurtBox.w;
        }

        flip = newFlip;
    }
};

inline SDL_Rect createClipFromSpriteSheet(
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

inline SDL_Rect scaleRect(SDL_Rect r, int scale)
{
    r.x *= scale;
    r.y *= scale;
    r.w *= scale;
    r.h *= scale;

    return r;
}

class Animation
{
    public:
        Animation(int fps, std::vector<Frame> &frames, bool repeats = true)
        : m_frames(frames)
        , m_accumulatedTime(0)
        , m_frameIndex(0)
        , m_flip(SDL_FLIP_NONE)
        , m_isDone(false)
        , m_repeats(repeats)
        {
            assert(!frames.empty() && "Animation must have at least one frame");
            setFPS(fps);
        }

        void setFlip(SDL_RendererFlip flip)
        {
            m_flip = flip;

            for (auto &frame : m_frames)
            {
                frame.setFlip(flip);
            }
        }

        void setFPS(int fps)
        {
            assert(fps > 0 && "FPS must be > 0");
            m_timePerFrame = 1000 / fps;
        }

        Frame getCurrentFrame()
        {
            return m_frames[m_frameIndex];
        }

        SDL_RendererFlip getFlip()
        {
            return m_flip;
        }

        bool isDone()
        {
            return m_isDone;
        }

        void update(int deltaTime)
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

        void reset()
        {
            m_frameIndex = 0;
            m_isDone = false;
            m_accumulatedTime = 0;
        }

    private:
        std::vector<Frame> m_frames;
        int m_timePerFrame;
        int m_accumulatedTime;
        int m_frameIndex;

        SDL_RendererFlip m_flip;
        bool m_isDone;
        bool m_repeats;
};

template<typename AnimationKeyType>
class AnimationManager
{
    public:
        void addAnimation(AnimationKeyType key, Animation &animation)
        {
            m_animations[key] = std::make_shared<Animation>(animation);
        }

        std::shared_ptr<Animation> getAnimation(AnimationKeyType key) const
        {
            return m_animations.at(key);
        }

    private:
        std::unordered_map<AnimationKeyType, std::shared_ptr<Animation>> m_animations;
};

#endif