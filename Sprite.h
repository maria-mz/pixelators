#ifndef SPRITE_H_
#define SPRITE_H_

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "SDL2/SDL.h"

constexpr int SECONDS_IN_MS = 1000;

// Compiler can't seem to find `makeSpriteClipper` without this alias...
using SpriteClipper = std::function<SDL_Rect(int, int, int, int, int, int)>;

SpriteClipper makeSpriteClipper(int originalWidth, int originalHeight, int scale);

class Animation
{
    public:
        Animation(int fps);

        void setFrames(std::vector<SDL_Rect> &frames);
        void setFlip(SDL_RendererFlip flip);

        SDL_Rect getCurrentFrame();
        SDL_RendererFlip getFlip();

        void update(int deltaTime);
        void reset();

    private:
        std::vector<SDL_Rect> m_frames;
        int m_timePerFrame;
        int m_accumulatedTime;
        int m_frameIndex;
        SDL_RendererFlip m_flip;
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