#include "RenderSystem.h"

RenderSystem::RenderSystem(SDL_Renderer *renderer, Player *player)
{
    m_renderer = renderer;
    m_player = player;
}

void RenderSystem::update()
{
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
    SDL_RenderClear(m_renderer);

    SDL_Rect clipQuad;
    SDL_Rect renderQuad;

    // Render player
    Animation *animation = m_player->m_sprite->getAnimator()->getCurrentAnimation();

    renderQuad.x = m_player->m_position->x;
    renderQuad.y = m_player->m_position->y;
    renderQuad.w = m_player->m_transform->width;
    renderQuad.h = m_player->m_transform->height;

    clipQuad = animation->getCurrentFrame();

    SDL_RenderCopyEx(m_renderer, animation->getTexture(), &clipQuad, &renderQuad, 0.0, NULL, animation->getFlip());

    SDL_RenderPresent(m_renderer);
}
