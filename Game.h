#ifndef GAME_H_
#define GAME_H_

#include <iostream>
#include <stdio.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "Player.h"
#include "RenderSystem.h"
#include "TextureManager.h"

class Game
{
    public:
        Game();

        bool init();
        void run();

        ~Game();

    private:
        bool initWindow();
        bool initRenderer();
        bool initTextures();

        SDL_Window *m_window;
        SDL_Renderer *m_renderer;
        Player *m_player;
        SDL_Texture *m_idle;
        SDL_Texture *m_running;
        RenderSystem *m_renderSystem;
        int m_gameTickRate;
};

#endif