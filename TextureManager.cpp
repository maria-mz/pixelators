#include "TextureManager.h"


TextureManager::~TextureManager()
{
    for (auto &pair : m_textures)
    {
        SDL_DestroyTexture(pair.second);
        pair.second = nullptr;
    }
}

bool TextureManager::loadTexture(std::string filePath, SDL_Renderer* renderer)
{
    removeTexture(filePath);

    bool success = true;

    SDL_Texture* newTexture = NULL;

    SDL_Surface* loadedSurface = IMG_Load(filePath.c_str());
    if (loadedSurface == NULL)
    {
        success = false;
        printf("Image failed to load! path=[%s] error=[%s]", filePath.c_str(), IMG_GetError());
    }
    else
    {
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (newTexture == NULL)
        {
            success = false;

            printf("Texture couldn't be created from image! path=[%s] error=[%s]",
                    filePath.c_str(),
                    IMG_GetError());
        }

        SDL_FreeSurface(loadedSurface);
    }

    if (newTexture)
    {
        m_textures[filePath] = newTexture;
    }

    return success;
}

SDL_Texture* TextureManager::getTexture(std::string filePath)
{
    if (m_textures.find(filePath) != m_textures.end())
    {
        return m_textures[filePath];
    }

    return nullptr;
}

void TextureManager::removeTexture(std::string filePath)
{
    SDL_Texture *texture = getTexture(filePath);
    if (texture)
    {
        SDL_DestroyTexture(texture);
        m_textures.erase(filePath);
    }
}
