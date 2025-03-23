#ifndef RENDER_SYSTEM_H_
#define RENDER_SYSTEM_H_

#include "ISystem.h"
#include "../Components/Components.h"
#include "SDL2/SDL.h"

class RenderSystem : public ISystem
{
    public:
        RenderSystem(SDL_Renderer* renderer);
        bool start() override;
        void input(SDL_Event &e) override;
        void update(int time) override;
        void end() override;

    private:
        SDL_Renderer* m_renderer;
};

#endif