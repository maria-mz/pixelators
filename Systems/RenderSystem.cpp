#include "RenderSystem.h"

RenderSystem::RenderSystem(SDL_Renderer* renderer)
{
    m_renderer = renderer;
}

bool RenderSystem::start()
{
    return true;
}

void RenderSystem::input(SDL_Event &e)
{

}

void RenderSystem::update(int time)
{
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
    SDL_RenderClear(m_renderer);

    SDL_Rect clipQuad;
    SDL_Rect renderQuad;

    SpriteComponent *sprite = m_scene->getPlayer()->spriteComponent();
    Animation *animation = sprite->getCurrentAnimation();
    TransformComponent *transform = m_scene->getPlayer()->transformComponent();
    VelocityComponent *velocity = m_scene->getPlayer()->velocityComponent();

    renderQuad.x = transform->posX();
    renderQuad.y = transform->posY();
    renderQuad.w = transform->width();
    renderQuad.h = transform->height();

    clipQuad = animation->getCurrentClip();

    if (velocity->velX() > 0)
    {
        SDL_RenderCopy(m_renderer, animation->getTexture(), &clipQuad, &renderQuad);
    }
    else
    {
        SDL_RenderCopyEx(m_renderer, animation->getTexture(), &clipQuad, &renderQuad, 0.0, NULL, SDL_FLIP_HORIZONTAL);
    }

    SDL_RenderPresent(m_renderer);
}

void RenderSystem::end()
{

}
