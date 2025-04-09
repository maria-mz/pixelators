#ifndef GAME_H_
#define GAME_H_

#include <iostream>
#include <stdio.h>
#include <chrono>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "Player.h"
#include "TextureManager.h"
#include "NetworkManager.h"

constexpr int MAX_OPPONENT_EVENTS_PER_TICK = 10;

class Game
{
    public:
        Game();

        bool init(bool isHost);
        void run();

        ~Game();

    private:
        bool initWindow();
        bool initRenderer();
        bool initTextures();

        void spawnPlayers();

        // todo: use smart pointers ?
        SDL_Window *m_window;
        SDL_Renderer *m_renderer;
        Player *m_player;
        Player *m_opponent;
        SDL_Texture *m_idle;
        SDL_Texture *m_running;
        int m_gameTickRate;

        bool m_isHost;

        std::unique_ptr<NetworkManager> m_network;
};

#endif