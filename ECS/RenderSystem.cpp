#include "RenderSystem.h"

RenderSystem::RenderSystem(SDL_Renderer* renderer)
{
    m_renderer = renderer;
}

bool RenderSystem::start()
{
    return true;
}

void RenderSystem::update(int time)
{
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
    SDL_RenderClear(m_renderer);

    SDL_Rect clipQuad;
    SDL_Rect renderQuad;

    for (auto &entity : m_entities)
    {
        if (entity->hasComponent<SpriteComponent>() && entity->hasComponent<TransformComponent>())
        {
            SpriteComponent *sprite = entity->getComponent<SpriteComponent>();
            TransformComponent *transform = entity->getComponent<TransformComponent>();

            renderQuad.x = transform->getPosX();
            renderQuad.y = transform->getPosY();
            renderQuad.w = transform->getWidth();
            renderQuad.h = transform->getHeight();

            clipQuad = sprite->getCurrentClip();

            SDL_RenderCopy(m_renderer, sprite->getTexture(), &clipQuad, &renderQuad);
        }
    }

    SDL_RenderPresent(m_renderer);
}

void RenderSystem::end()
{

}
