#ifndef TEXT_H_
#define TEXT_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "Resources.h"

class Text
{
    public:
        Text(TTF_Font *font, SDL_Color color) : m_texture(nullptr), m_font(font), m_color(color) {}

        ~Text()
        {
            destroyText();
        }

        void setText(const char *text, int outlinePx = 0, SDL_Color outlineColor = {0, 0, 0})
        {
            destroyText();

            if (outlinePx > 0)
            {
                int originalOutline = TTF_GetFontOutline(m_font);

                TTF_SetFontOutline(m_font, outlinePx);
                SDL_Surface* outlineSurface = TTF_RenderText_Blended(m_font, text, outlineColor);

                TTF_SetFontOutline(m_font, originalOutline); // reset outline on font

                SDL_Surface* mainSurface = TTF_RenderText_Blended(m_font, text, m_color);

                // Blit main text onto outlined background
                SDL_BlitSurface(mainSurface, NULL, outlineSurface, NULL);

                m_texture = SDL_CreateTextureFromSurface(Resources::renderer, outlineSurface);
                m_surfaceWidth = outlineSurface->w;
                m_surfaceHeight = outlineSurface->h;

                SDL_FreeSurface(outlineSurface);
                SDL_FreeSurface(mainSurface);
            }
            else
            {
                SDL_Surface* mainSurface = TTF_RenderText_Blended(m_font, text, m_color);

                m_texture = SDL_CreateTextureFromSurface(Resources::renderer, mainSurface);
                m_surfaceWidth = mainSurface->w;
                m_surfaceHeight = mainSurface->h;

                SDL_FreeSurface(mainSurface);
            }
        }

        int getWidth()
        {
            return m_surfaceWidth;
        }

        int getHeight()
        {
            return m_surfaceHeight;
        }

        void render(int x, int y)
        {
            SDL_Rect dstRect = {x, y, m_surfaceWidth, m_surfaceHeight};
            SDL_RenderCopy(Resources::renderer, m_texture, NULL, &dstRect);
        }

    private:
        void destroyText()
        {
            SDL_DestroyTexture(m_texture);
        }

        SDL_Texture *m_texture;
        TTF_Font *m_font;
        SDL_Color m_color;
        int m_surfaceWidth;
        int m_surfaceHeight;
};

#endif