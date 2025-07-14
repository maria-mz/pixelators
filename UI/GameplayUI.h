#ifndef GAMEPLAY_UI_H_
#define GAMEPLAY_UI_H_

#include "SDL2/SDL.h"

#include "../Utils/Utils.h"
#include "../Constants.h"
#include "HealthBar.h"

class GameplayUI
{
    public:
        void render(int playerOneHealth, int playerTwoHealth)
        {
            if (computeHP(playerOneHealth, Constants::PLAYER_MAX_HEALTH) <= Constants::PLAYER_LOW_HP)
            {
                m_playerOneHealthBar.setType(HealthBarType::Low);
            }

            if (computeHP(playerTwoHealth, Constants::PLAYER_MAX_HEALTH) <= Constants::PLAYER_LOW_HP)
            {
                m_playerTwoHealthBar.setType(HealthBarType::Low);
            }

            m_playerOneHealthBar.render(playerOneHealth, 20, 20, 300, 20, false);
            m_playerTwoHealthBar.render(playerTwoHealth, 400, 20, 300, 20, true);

            renderPlayerProfile(20, 40, Resources::textures.getTexture(Constants::FILE_SPRITE_PLAYER_PROFILE_RED));
            renderPlayerProfile(636, 40, Resources::textures.getTexture(Constants::FILE_SPRITE_PLAYER_PROFILE_BLUE), SDL_FLIP_HORIZONTAL);
        }

    private:
        void renderPlayerProfile(int x, int y, SDL_Texture *texture, SDL_RendererFlip flip = SDL_FLIP_NONE)
        {
            SDL_Rect clipRect = scaleRect({10, 9, 15, 15}, 15);
            SDL_Rect renderRect = {x, y, 64, 64};

            SDL_RenderCopyEx(
                Resources::renderer,
                texture,
                &clipRect,
                &renderRect,
                0.0,
                NULL,
                flip
            );
        }

        HealthBar m_playerOneHealthBar;
        HealthBar m_playerTwoHealthBar;
};

#endif