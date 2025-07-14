#ifndef GAMEPLAY_UI_H_
#define GAMEPLAY_UI_H_

#include "SDL2/SDL.h"

#include "../Utils/Utils.h"
#include "HealthBar.h"

class GameplayUI
{
    public:
        void init()
        {
            m_playerOneHealthBar.initHealthBar();
            m_playerTwoHealthBar.initHealthBar();
        }

        void setLowHP(int lowHP)
        {
            m_lowHP = lowHP;
        }

        void setMaxHealth(int maxHealth)
        {
            m_maxHealth = maxHealth;
        }

        void render(int playerOneHealth, int playerTwoHealth)
        {
            if (computeHP(playerOneHealth, m_maxHealth) <= m_lowHP)
            {
                m_playerOneHealthBar.setType(HealthBarType::Low);
            }

            if (computeHP(playerTwoHealth, m_maxHealth) <= m_lowHP)
            {
                m_playerTwoHealthBar.setType(HealthBarType::Low);
            }

            m_playerOneHealthBar.render(playerOneHealth, m_maxHealth, 16, 20, 304, 16, false);
            m_playerTwoHealthBar.render(playerTwoHealth, m_maxHealth, 400, 20, 304, 16, true);

            renderPlayerProfile(25, 35, Resources::textures.getTexture(Constants::FILE_SPRITE_PLAYER_PROFILE_RED));
            renderPlayerProfile(630, 35, Resources::textures.getTexture(Constants::FILE_SPRITE_PLAYER_PROFILE_BLUE), SDL_FLIP_HORIZONTAL);
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

        int m_lowHP;
        int m_maxHealth;
};

#endif