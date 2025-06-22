#ifndef BUTTON_H_
#define BUTTON_H_

#include <functional>
#include <string>
#include <SDL2/SDL.h>

#include "Text.h"
#include "Resources.h"

class Button {
    public:
        Button() {}

        Button(int x, int y, int w, int h, SDL_Color color) : m_rect({x, y, w, h}), m_color(color) {}

        void setText(std::unique_ptr<Text> text)
        {
            m_text = std::move(text);
        }

        void setOnClick(std::function<void()> callback)
        {
            m_onClick = std::move(callback);
        }

        void render() {
            SDL_SetRenderDrawColor(Resources::renderer, m_color.r, m_color.g, m_color.b, 255);
            SDL_RenderFillRect(Resources::renderer, &m_rect);

            if (m_text)
            {
                m_text->render(m_rect.x + (m_rect.w - m_text->getWidth()) / 2,
                               m_rect.y + (m_rect.h - m_text->getHeight()) / 2);
            }
        }

        void handleEvent(const SDL_Event& e) {
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
            {
                int mx = e.button.x;
                int my = e.button.y;
                if (mx >= m_rect.x && mx <= m_rect.x + m_rect.w &&
                    my >= m_rect.y && my <= m_rect.y + m_rect.h)
                {
                    if (m_onClick)
                    {
                        m_onClick();
                    }
                }
            }
        }

    private:
        SDL_Rect m_rect;
        SDL_Color m_color;
        std::unique_ptr<Text> m_text;

        std::function<void()> m_onClick = nullptr;
};

#endif