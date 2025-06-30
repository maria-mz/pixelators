#ifndef MENU_UI_H
#define MENU_UI_H

#include <format>

#include "SDL2/SDL.h"

#include "../Constants.h"
#include "../Resources.h"
#include "../Utils/Utils.h"
#include "Text.h"
#include "Button.h"
#include "TextField.h"

class MenuUI
{
    public:
        void init()
        {
            initHostGameButton();
            initJoinGameButton();
            initHostIPTextField();
            initIPAddressHeader();
        }

        // void setOnPlayButtonClick(std::function<void()> callback)
        // {
        //     m_playButton.setOnClick(callback);
        // }

        void handleEvent(const SDL_Event &e)
        {
            m_hostGameButton.handleEvent(e);
            m_joinGameButton.handleEvent(e);
            m_hostIPTextField.handleEvent(e);
        }

        void render()
        {
            m_hostGameButton.render();
            m_IPAddressHeader->render(m_hostIPTextField.getX(),
                                      m_hostIPTextField.getY() - m_IPAddressHeader->getHeight() - 5);
            m_hostIPTextField.render();
            m_joinGameButton.render();
        }

    private:
        void initHostGameButton()
        {
            TextConfig textConfig{Resources::fonts.getFont(Constants::FILE_FONT_MAIN, 12),
                                  {255, 255, 255}};

            int w = 200;
            int h = 50;

            int x = (Constants::WINDOW_WIDTH - w) / 2;
            int y = 100;

            m_hostGameButton= Button(x, y, w, h);

            m_hostGameButton.setText("Host Game", textConfig);

            m_hostGameButton.setMainColor({197, 58, 93});
            m_hostGameButton.setShadowColor({197, 58, 93});
            m_hostGameButton.setHoverColor({239, 71, 111});
        }

        void initJoinGameButton()
        {
            TextConfig textConfig{Resources::fonts.getFont(Constants::FILE_FONT_MAIN, 12),
                                  {255, 255, 255}};

            int w = 200;
            int h = 50;

            int x = (Constants::WINDOW_WIDTH - w) / 2;
            int y = 330;

            m_joinGameButton = Button(x, y, w, h);

            m_joinGameButton.setText("Join Game", textConfig);

            m_joinGameButton.setMainColor({58, 172, 197});
            m_joinGameButton.setShadowColor({58, 172, 197});
            m_joinGameButton.setHoverColor({66, 203, 223});
        }

        void initIPAddressHeader()
        {
            TextConfig textConfig{Resources::fonts.getFont(Constants::FILE_FONT_MAIN, 12),
                                  {255, 255, 255}};

            m_IPAddressHeader = std::make_unique<Text>(textConfig);
            m_IPAddressHeader->setText("IP Address");
        }

        void initHostIPTextField()
        {
            TTF_Font *textFont = Resources::fonts.getFont(Constants::FILE_FONT_MAIN, 12);

            int w = 200;
            int h = 50;

            int x = (Constants::WINDOW_WIDTH - w) / 2;
            int y = 270;

            m_hostIPTextField = TextField(x, y, w, h, textFont);
            m_hostIPTextField.setTextHint("192.168.0.1");
        }

        // std::function<void()> m_onPlayButtonClick = nullptr;

        Button m_hostGameButton;
        Button m_joinGameButton;
        std::unique_ptr<Text> m_IPAddressHeader;
        TextField m_hostIPTextField;
};

#endif