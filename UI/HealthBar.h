#ifndef HEALTH_BAR_H_
#define HEALTH_BAR_H_

#include "SDL2/SDL.h"

#include "../Constants.h"
#include "../Resources.h"
#include "../Utils/Utils.h"

enum class HealthBarType
{
    Ok,
    Low
};

class HealthBar
{
    public:
        void setType(HealthBarType type)
        {
            m_type = type;
        }

        void render(int health,
                    int x,
                    int y,
                    int w,
                    int h,
                    bool flipHorizontal = false)
        {
            SDL_Rect bgRect = {x, y, w, h};
            SDL_Rect fillRect;

            int fillWidth = (computeHP(health, Constants::PLAYER_MAX_HEALTH) / 100.0f) * w;

            if (flipHorizontal)
            {
                fillRect = {x, y, fillWidth, h};
            }
            else
            {
                fillRect = {x + w - fillWidth, y, fillWidth, h};
            }

            SDL_Color fillColor = getFillColor(m_type);
            SDL_Color bgColor = getBgColor(m_type);

            SDL_SetRenderDrawColor(Resources::renderer, bgColor.r, bgColor.g, bgColor.b, 128);
            SDL_RenderFillRect(Resources::renderer, &bgRect);

            SDL_SetRenderDrawColor(Resources::renderer, fillColor.r, fillColor.g, fillColor.b, 255);
            SDL_RenderFillRect(Resources::renderer, &fillRect);

            // draw outline
            SDL_SetRenderDrawColor(Resources::renderer, bgColor.r, bgColor.g, bgColor.b, 255);
            SDL_RenderDrawRect(Resources::renderer, &bgRect);
        }

    private:
        SDL_Color getFillColor(HealthBarType type)
        {
            switch (type)
            {
                case HealthBarType::Ok:
                    return {19, 197, 63};
                case HealthBarType::Low:
                    return {197, 19, 69};
            }
        }

        SDL_Color getBgColor(HealthBarType type)
        {
            switch (type)
            {
                case HealthBarType::Ok:
                    return {22, 51, 29};
                case HealthBarType::Low:
                    return {51, 22, 30};
            }
        }

        HealthBarType m_type = HealthBarType::Ok;
};

#endif