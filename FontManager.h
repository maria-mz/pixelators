#ifndef FONT_MANAGER_H_
#define FONT_MANAGER_H_

#include <map>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "Logging.h"

class FontManager
{
    public:
        ~FontManager();

        bool loadFont(const char* file, int fontSizePx, SDL_Renderer* renderer);
        TTF_Font* getFont(const char* file, int fontSizePx);
        void removeFont(const char* file, int fontSizePx);

    private:
        std::map<std::pair<const char*, int>, TTF_Font*> m_fonts;
};

#endif