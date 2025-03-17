#ifndef SPRITE_COMPONENT_H_
#define SPRITE_COMPONENT_H_

#include "SDL2/SDL.h"
#include <vector>

constexpr int SECONDS_IN_MS = 1000;

class SpriteComponent
{
    public:
        SpriteComponent();
        SpriteComponent(int animationFPS);

        void setTexture(SDL_Texture* texture);
        SDL_Texture* getTexture();

        void addClip(SDL_Rect spriteClip);

        SDL_Rect getCurrentClip();
        SDL_Rect getClipAtIndex(int index);

        bool isAnimated();
        int getTimePerClip();
        int getNumClips();
        void setClipIndex(int index);

    private:
        std::vector<SDL_Rect> m_spriteClips;
        SDL_Texture* m_texture;

        int m_timePerClip;
        int m_clipIndex;
};

#endif