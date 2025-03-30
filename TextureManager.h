#ifndef TEXTURE_MANAGER_H_
#define TEXTURE_MANAGER_H_

#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class TextureManager
{
    public:
        static SDL_Texture* loadTexture(std::string filePath, SDL_Renderer* renderer);
};

#endif