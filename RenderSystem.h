#ifndef RENDER_SYSTEM_H_
#define RENDER_SYSTEM_H_

#include "SDL2/SDL.h"
#include "Player.h"

class RenderSystem
{
    public:
        RenderSystem(SDL_Renderer *renderer, Player *player);
        void update();

    private:
        SDL_Renderer *m_renderer;
        Player *m_player;
};

#endif