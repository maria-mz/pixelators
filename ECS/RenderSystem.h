#ifndef RENDER_SYSTEM_H_
#define RENDER_SYSTEM_H_

#include "ECS.h"
#include "Components.h"
#include "SDL2/SDL.h"

class RenderSystem : public ISystem
{
    public:
        RenderSystem(SDL_Renderer* renderer);
        bool start() override;
        void update(int time) override;
        void end() override;

    private:
        SDL_Renderer* m_renderer;
};

#endif