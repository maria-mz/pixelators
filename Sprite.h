#ifndef SPRITE_H_
#define SPRITE_H_

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "SDL2/SDL.h"

constexpr int SECONDS_IN_MS = 1000;

class Frame
{
    public:
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

SDL_Rect createClipFromSpriteSheet(
    int spriteWidth,
    int spriteHeight,
    int pixelsFromLeft,
    int pixelsFromTop,
    int entityWidth,
    int entityHeight,
    int row,
    int col
);

SDL_Rect scaleRect(SDL_Rect r, int scale);

class Animation
{
    public:
        Animation(int fps, bool repeats = true);

        void setFrames(std::vector<Frame> &frames);
        void setFlip(SDL_RendererFlip flip);

        Frame getCurrentFrame();
        SDL_RendererFlip getFlip();
        bool isDone();

        void update(int deltaTime);
        void reset();

    private:
        std::vector<Frame> m_frames;
        int m_timePerFrame;
        int m_accumulatedTime;
        int m_frameIndex;
        SDL_RendererFlip m_flip;

        bool m_isDone = false;
        bool m_repeats;
};

class Animator
{
    public:
        Animator();
        ~Animator();

        void addAnimation(int animationTag, Animation *animation);
        void setCurrentAnimation(int animationTag);
        void updateCurrentAnimation(int deltaTime);
        Animation *getCurrentAnimation();
        Animation *getAnimation(int animationTag);

    private:
        std::unordered_map<int, Animation*> m_animations;
        int m_currentAnimationTag;
};

class Sprite
{
    public:
        Sprite();
        ~Sprite();

        void setAnimator(Animator *animator);
        Animator *getAnimator();

    private:
        Animator *m_animator;
};

#endif