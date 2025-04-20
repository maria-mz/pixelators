#ifndef TEXTURE_MANAGER_H_
#define TEXTURE_MANAGER_H_

#include <map>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


class TextureManager
{
    public:
        ~TextureManager();

        bool loadTexture(std::string filePath, SDL_Renderer* renderer);
        SDL_Texture* getTexture(std::string filePath);
        void removeTexture(std::string filePath);

    private:
        std::map<std::string, SDL_Texture*> m_textures;
};

#endif