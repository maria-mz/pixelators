#ifndef GAME_H_
#define GAME_H_

#include <iostream>
#include <stdio.h>
#include <chrono>
#include <thread>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "Player.h"
#include "TextureManager.h"
#include "NetworkManager.h"

constexpr const int GAME_TICK_RATE_MS = 16; // ~16 ms per tick (~60 updates per second)

const Vector2D PLAYER_1_SPAWN_POSITION(180, 200);
const Vector2D PLAYER_2_SPAWN_POSITION(540, 200);

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

        void handleOpponentMsg(GameMessage &msg);
        void sendPlayerMovement();

        // todo: use smart pointers
        SDL_Window *m_window;
        SDL_Renderer *m_renderer;
        Player *m_player;
        Player *m_opponent;
        SDL_Texture *m_idle;
        SDL_Texture *m_running;

        bool m_isHost;

        std::unique_ptr<NetworkManager> m_network;
};

#endif