#ifndef GAMEPLAY_UI_H_
#define GAMEPLAY_UI_H_

#include "SDL2/SDL.h"

#include "../Utils/Utils.h"
#include "../Constants.h"
#include "HealthBar.h"
#include "Text.h"

class GameplayUI
{
    public:
        void init()
        {
            TextConfig textConfig{Resources::fonts.getFont(Constants::FILE_FONT_MAIN, 12),
                                  {255, 255, 255},
                                  2};

            m_player1Text = std::make_unique<Text>(textConfig);
            m_player2Text = std::make_unique<Text>(textConfig);

            m_player1Text->setText("P1");
            m_player2Text->setText("P2");
        }

        void render(int player1Health, int player2Health)
        {
            if (computeHP(player1Health, Constants::PLAYER_MAX_HEALTH) <= Constants::PLAYER_LOW_HP)
            {
                m_player1HealthBar.setType(HealthBarType::Low);
            }

            if (computeHP(player2Health, Constants::PLAYER_MAX_HEALTH) <= Constants::PLAYER_LOW_HP)
            {
                m_player2HealthBar.setType(HealthBarType::Low);
            }

            m_player1HealthBar.render(player1Health, 20, 20, 300, 20, false);
            m_player2HealthBar.render(player2Health, 400, 20, 300, 20, true);

            renderPlayerProfile(20, 40, Resources::textures.getTexture(Constants::FILE_SPRITE_PLAYER_PROFILE_RED));
            renderPlayerProfile(636, 40, Resources::textures.getTexture(Constants::FILE_SPRITE_PLAYER_PROFILE_BLUE), SDL_FLIP_HORIZONTAL);

            m_player1Text->render(84, 50);
            m_player2Text->render(636 - m_player2Text->getWidth(), 50);
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

        HealthBar m_player1HealthBar;
        HealthBar m_player2HealthBar;

        std::unique_ptr<Text> m_player1Text;
        std::unique_ptr<Text> m_player2Text;
};

#endif