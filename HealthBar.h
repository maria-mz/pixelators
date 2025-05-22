#ifndef HEALTH_BAR_H_
#define HEALTH_BAR_H_

#include <format>

#include "SDL2/SDL.h"

#include "Constants.h"
#include "Resources.h"
#include "Sprite.h"
#include "Text.h"

enum class HealthBarType
{
    Ok,
    Low
};

class HealthBar
{
    const SDL_Rect HEALTH_BAR_FILL_EDGE_RECT = scaleRect(createClipFromSpriteSheet(8, 8, 2, 2, 4, 4, 0, 0), 15);
    const SDL_Rect HEALTH_BAR_FILL_BODY_RECT = scaleRect(createClipFromSpriteSheet(8, 8, 2, 2, 4, 4, 0, 1), 15);
    const SDL_Rect HEALTH_BAR_BG_EDGE_RECT = scaleRect(createClipFromSpriteSheet(8, 8, 2, 2, 4, 4, 1, 0), 15);
    const SDL_Rect HEALTH_BAR_BG_BODY_RECT = scaleRect(createClipFromSpriteSheet(8, 8, 2, 2, 4, 4, 1, 1), 15);

    public:
        HealthBar()
        {
            TTF_Font *healthTextFont = Resources::fonts.getFont(Constants::FILE_FONT_MAIN, 8);
            SDL_Color healthTextColor = {255, 255, 255};

            m_text = std::make_unique<Text>(healthTextFont, healthTextColor);
            m_type = HealthBarType::Ok;
        }

        void setType(HealthBarType type)
        {
            m_type = type;
        }

        void render(int health,
                    int maxHealth,
                    int x,
                    int y,
                    int w,
                    int h,
                    bool flipHorizontal = false)
        {
            assert(w % h == 0 && "Health bar width must be a multiple of height");

            SDL_Texture *texture = getHealthBarTexture(m_type);

            int chunks = w / h;
            int chunkWidth = w / chunks;

            int filledChunks = (computeHP(health, maxHealth) / 100.0f) * chunks;

            SDL_Rect dstRect = {x, y, chunkWidth, h};

            if (!flipHorizontal)
            {
                for (int i = 0; i < chunks - filledChunks; i++)
                {
                    if (i == 0)
                    {
                        SDL_RenderCopy(Resources::renderer, texture, &HEALTH_BAR_BG_EDGE_RECT, &dstRect);
                    }
                    else
                    {
                        SDL_RenderCopy(Resources::renderer, texture, &HEALTH_BAR_BG_BODY_RECT, &dstRect);
                    }

                    dstRect.x += chunkWidth;
                }

                for (int i = chunks - filledChunks; i < chunks; i++)
                {
                    if (i == chunks - filledChunks)
                    {
                        SDL_RenderCopy(Resources::renderer, texture, &HEALTH_BAR_BG_BODY_RECT, &dstRect);
                        SDL_RenderCopy(Resources::renderer, texture, &HEALTH_BAR_FILL_EDGE_RECT, &dstRect);
                    }
                    else
                    {
                        SDL_RenderCopy(Resources::renderer, texture, &HEALTH_BAR_FILL_BODY_RECT, &dstRect);
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
                        SDL_RenderCopyEx(Resources::renderer, texture, &HEALTH_BAR_BG_BODY_RECT, &dstRect, 0.0, NULL, SDL_FLIP_HORIZONTAL);
                        SDL_RenderCopyEx(Resources::renderer, texture, &HEALTH_BAR_FILL_EDGE_RECT, &dstRect, 0.0, NULL, SDL_FLIP_HORIZONTAL);
                    }
                    else
                    {
                        SDL_RenderCopy(Resources::renderer, texture, &HEALTH_BAR_FILL_BODY_RECT, &dstRect);
                    }

                    dstRect.x += chunkWidth;
                }

                for (int i = filledChunks; i < chunks; i++)
                {
                    if (i == chunks - 1)
                    {
                        SDL_RenderCopyEx(Resources::renderer, texture, &HEALTH_BAR_BG_EDGE_RECT, &dstRect, 0.0, NULL, SDL_FLIP_HORIZONTAL);
                    }
                    else
                    {
                        SDL_RenderCopy(Resources::renderer, texture, &HEALTH_BAR_BG_BODY_RECT, &dstRect);
                    }

                    dstRect.x += chunkWidth;
                }
            }

            renderHealthText(x, y, w, h, health, maxHealth, flipHorizontal);
        }

    private:
        SDL_Texture* getHealthBarTexture(HealthBarType type)
        {
            switch (type)
            {
                case HealthBarType::Ok:
                    return Resources::textures.getTexture(Constants::FILE_SPRITE_HEALTH_BAR_OK);
                case HealthBarType::Low:
                    return Resources::textures.getTexture(Constants::FILE_SPRITE_HEALTH_BAR_LOW);
            }
        }

        void renderHealthText(int barX,
                              int barY,
                              int barWidth,
                              int barHeight,
                              int health,
                              int maxHealth,
                              bool flipHorizontal = false)
        {
            if (health != m_health || maxHealth != m_maxHealth)
            {
                char text[16];
                int textOulinePx = 1;
                std::snprintf(text, sizeof(text), "%d / %d", health, maxHealth);

                m_text->setText(text, textOulinePx);

                m_health = health;
                m_maxHealth = maxHealth;
            }

            int paddingHorizontalPx = 4;
            int paddingVerticalPx = m_text->getHeight() >= barHeight ? 0 : (barHeight - m_text->getHeight()) / 2.0f;

            int textX = !flipHorizontal ? barX + barWidth - paddingHorizontalPx - m_text->getWidth() : barX + paddingHorizontalPx;
            int textY = barY + paddingVerticalPx;

            m_text->render(textX, textY);
        }

        int computeHP(int health, int maxHealth)
        {
            return health * 100.0f / maxHealth;
        }

        HealthBarType m_type;
        std::unique_ptr<Text> m_text;

        int m_health;
        int m_maxHealth;
};

#endif