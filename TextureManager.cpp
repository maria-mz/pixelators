#include "TextureManager.h"

SDL_Texture* TextureManager::loadTexture(std::string filePath, SDL_Renderer* renderer)
{
    SDL_Texture* newTexture = NULL;

    SDL_Surface* loadedSurface = IMG_Load(filePath.c_str());
    if (loadedSurface == NULL)
    {
        printf("Image failed to load! path=[%s] error=[%s]", filePath.c_str(), IMG_GetError());
    }
    else
    {
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (newTexture == NULL)
        {
            printf("Texture couldn't be created from image! path=[%s] error=[%s]",
                   filePath.c_str(),
                   IMG_GetError());
        }

        SDL_FreeSurface(loadedSurface);
    }

    return newTexture;
}
