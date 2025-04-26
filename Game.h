#ifndef GAME_H_
#define GAME_H_

#include <iostream>
#include <stdio.h>
#include <deque>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "Logging.h"
#include "Player.h"
#include "Resources.h"
#include "NetworkManager.h"
#include "Netcode.h"
#include "FrameTimer.h"

// Uncomment to see bounding, hit, and hurt boxes
#define DEBUG_MODE

constexpr const int GAME_TICK_RATE_MS = 16; // ~16 ms per frame (~60 updates per second)

const Vector2D PLAYER_1_SPAWN_POSITION(180, 200);
const Vector2D PLAYER_2_SPAWN_POSITION(540, 200);

constexpr const int MIN_OPPONENT_LAG_FRAMES = 1;

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

        void handleOpponentNetMsgs();
        void updateOpponent(int deltaTime);

        void render();

        SDL_Window *m_window;
        SDL_Renderer *m_renderer;

        std::unique_ptr<Player> m_player;
        std::unique_ptr<Player> m_opponent;

        bool m_isHost;

        std::shared_ptr<NetworkManager> m_network;
        Netcode m_opponentNetcode;
        std::deque<GameMessage> m_opponentUpdatesBuffer;
};

#endif