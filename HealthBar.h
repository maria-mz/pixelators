#ifndef HEALTH_BAR_H_
#define HEALTH_BAR_H_

#include "SDL2/SDL.h"
#include "Constants.h"
#include "Resources.h"
#include "Sprite.h"

const SDL_Rect HEALTH_BAR_FILL_EDGE_RECT = scaleRect(createClipFromSpriteSheet(8, 8, 2, 2, 4, 4, 0, 0), 15);
const SDL_Rect HEALTH_BAR_FILL_BODY_RECT = scaleRect(createClipFromSpriteSheet(8, 8, 2, 2, 4, 4, 0, 1), 15);
const SDL_Rect HEALTH_BAR_BG_EDGE_RECT = scaleRect(createClipFromSpriteSheet(8, 8, 2, 2, 4, 4, 1, 0), 15);
const SDL_Rect HEALTH_BAR_BG_BODY_RECT = scaleRect(createClipFromSpriteSheet(8, 8, 2, 2, 4, 4, 1, 1), 15);

enum class HealthBarType
{
    Ok,
    Low
};

inline SDL_Texture* getHealthBarTexture(HealthBarType type)
{
    switch (type)
    {
        case HealthBarType::Ok:
            return Resources::textures.getTexture(Constants::FILE_SPRITE_HEALTH_BAR_OK);
        case HealthBarType::Low:
            return Resources::textures.getTexture(Constants::FILE_SPRITE_HEALTH_BAR_LOW);
    }
}

inline void renderHealthBar(
    int x,
    int y,
    int w,
    int h,
    SDL_Renderer *renderer,
    int hp,
    HealthBarType type,
    bool flipHorizontal = false
)
{
    SDL_Texture *texture = getHealthBarTexture(type);

    assert(w % h == 0 && "Health bar width must be a multiple of height");

    int chunks = w / h;
    int chunkWidth = w / chunks;

    int filledChunks = (hp / 100.0f) * chunks;

    SDL_Rect dstRect = {x, y, chunkWidth, h};

    if (!flipHorizontal)
    {
        for (int i = 0; i < chunks - filledChunks; i++)
        {
            if (i == 0)
            {
                SDL_RenderCopy(renderer, texture, &HEALTH_BAR_BG_EDGE_RECT, &dstRect);
            }
            else
            {
                SDL_RenderCopy(renderer, texture, &HEALTH_BAR_BG_BODY_RECT, &dstRect);
            }

            dstRect.x += chunkWidth;
        }

        for (int i = chunks - filledChunks; i < chunks; i++)
        {
            if (i == chunks - filledChunks)
            {
                SDL_RenderCopy(renderer, texture, &HEALTH_BAR_BG_BODY_RECT, &dstRect);
                SDL_RenderCopy(renderer, texture, &HEALTH_BAR_FILL_EDGE_RECT, &dstRect);
            }
            else
            {
                SDL_RenderCopy(renderer, texture, &HEALTH_BAR_FILL_BODY_RECT, &dstRect);
            }

            dstRect.x += chunkWidth;
        }
    }
    else
    {
        for (int i = 0; i < filledChunks; i++)
        {
            if (i == filledChunks - 1)
            {
                SDL_RenderCopyEx(renderer, texture, &HEALTH_BAR_BG_BODY_RECT, &dstRect, 0.0, NULL, SDL_FLIP_HORIZONTAL);
                SDL_RenderCopyEx(renderer, texture, &HEALTH_BAR_FILL_EDGE_RECT, &dstRect, 0.0, NULL, SDL_FLIP_HORIZONTAL);
            }
            else
            {
                SDL_RenderCopy(renderer, texture, &HEALTH_BAR_FILL_BODY_RECT, &dstRect);
            }

            dstRect.x += chunkWidth;
        }

        for (int i = filledChunks; i < chunks; i++)
        {
            if (i == chunks - 1)
            {
                SDL_RenderCopyEx(renderer, texture, &HEALTH_BAR_BG_EDGE_RECT, &dstRect, 0.0, NULL, SDL_FLIP_HORIZONTAL);
            }
            else
            {
                SDL_RenderCopy(renderer, texture, &HEALTH_BAR_BG_BODY_RECT, &dstRect);
            }

            dstRect.x += chunkWidth;
        }
    }
}

#endif