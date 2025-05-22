#ifndef RESOURCES_H_
#define RESOURCES_H_

#include "TextureManager.h"
#include "FontManager.h"
#include <SDL2/SDL.h>

namespace Resources
{
    inline TextureManager textures;
    inline FontManager fonts;
    inline SDL_Renderer* renderer;
}

#endif