#ifndef MENU_UI_H
#define MENU_UI_H

#include <format>

#include "SDL2/SDL.h"

#include "Constants.h"
#include "Resources.h"
#include "Sprite.h"
#include "Text.h"
#include "Button.h"
#include "Utils/Utils.h"

class MenuUI
{
    public:
        void init()
        {
            initPlayButton();
        }

        void setOnPlayButtonClick(std::function<void()> callback)
        {
            m_playButton.setOnClick(callback);
        }

        void handleEvent(const SDL_Event &e)
        {
            m_playButton.handleEvent(e);
        }

        void render()
        {
            m_playButton.render();
        }

    private:
        void initPlayButton()
        {
            TTF_Font *textFont = Resources::fonts.getFont(Constants::FILE_FONT_MAIN, 18);
            SDL_Color textColor = {255, 255, 255};

            std::unique_ptr<Text> text = std::unique_ptr<Text>(new Text(textFont, textColor));
            text->setText("Play Game");

            int w = 200;
            int h = 50;

            int x = (Constants::WINDOW_WIDTH - w) / 2;
            int y = (Constants::WINDOW_HEIGHT - h) / 2;

            SDL_Color buttonColor = {0, 0, 0};

            m_playButton = Button(x, y, w, h, buttonColor);
            m_playButton.setText(std::move(text));
        }

        Button m_playButton;
        std::function<void()> m_onPlayButtonClick = nullptr;
};

#endif