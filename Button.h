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

        void setColor(SDL_Color color)
        {
            m_color = color;
        }

        void setOnHoverIn(std::function<void()> callback)
        {
            m_onHoverIn = std::move(callback);
        }

        void setOnHoverOut(std::function<void()> callback)
        {
            m_onHoverOut = std::move(callback);
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
            if (e.type == SDL_MOUSEMOTION)
            {
                int mx = e.motion.x;
                int my = e.motion.y;
                bool wasHovered = m_isHover;

                m_isHover = (mx >= m_rect.x && mx <= m_rect.x + m_rect.w &&
                             my >= m_rect.y && my <= m_rect.y + m_rect.h);

                if (!wasHovered && m_isHover && m_onHoverIn)
                {
                    m_onHoverIn();
                }
                else if (wasHovered && !m_isHover && m_onHoverOut)
                {
                    m_onHoverOut();
                }

            }
            else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
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

        bool m_isHover;

        std::function<void()> m_onClick = nullptr;
        std::function<void()> m_onHoverIn = nullptr;
        std::function<void()> m_onHoverOut = nullptr;
};

#endif