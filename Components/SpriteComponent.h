#ifndef SPRITE_COMPONENT_H_
#define SPRITE_COMPONENT_H_

#include "SDL2/SDL.h"
#include <vector>
#include <map>

constexpr int SECONDS_IN_MS = 1000;

class Animation
{
    public:
        Animation(int animationFPS);

        void setTexture(SDL_Texture* texture);
        SDL_Texture* getTexture();

        void addClip(SDL_Rect spriteClip);

        SDL_Rect getCurrentClip();
        SDL_Rect getClipAtIndex(int index);

        int getTimePerClip();
        int getNumClips();
        void setClipIndex(int index);

        void reset();

    private:
        std::vector<SDL_Rect> m_spriteClips;
        SDL_Texture* m_texture;

        int m_timePerClip;
        int m_clipIndex;
};

class SpriteComponent
{
    public:
        SpriteComponent();
        ~SpriteComponent();

        void addAnimation(std::string tag, Animation *animation);
        Animation* getCurrentAnimation();
        void setCurrentAnimation(std::string tag);

    private:
        std::map<std::string, Animation*> m_animations;
        std::string m_currentTag;
};

#endif