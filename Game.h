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
#include "HealthBar.h"
#include "Utils/Utils.h"

// Uncomment to see bounding, hit, and hurt boxes
#define DEBUG_MODE

constexpr const int GAME_TICK_RATE_MS = 16; // ~16 ms per frame (~60 updates per second)

const Vector2D PLAYER_1_SPAWN_POSITION(180, 200);
const Vector2D PLAYER_2_SPAWN_POSITION(540, 200);

constexpr const int PLAYER_1_ID = 1;
constexpr const int PLAYER_2_ID = 2;

constexpr const int MIN_OPPONENT_LAG_FRAMES = 1;

constexpr const int LOW_HP = 30;

enum class GameState
{
    Menu,
    Gameplay
};

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

        void sendPlayerMovementUpdate(const MovementUpdate &movementUpdate);
        void sendHitRegistered(const HitRegistered &hitRegistered);

        void handleEvent(const SDL_Event &event);
        void update(const int deltaTime);

        void render();
        void renderGameplay();
        void renderPlayer(std::shared_ptr<Player> player);

        SDL_Window *m_window;
        SDL_Renderer *m_renderer;

        std::shared_ptr<Player> m_player;
        std::shared_ptr<Player> m_opponent;
        std::shared_ptr<HealthBar> m_playerHealthBar;
        std::shared_ptr<HealthBar> m_opponentHealthBar;

        InputEvent m_playerInputEvent;

        bool m_isHost;

        std::shared_ptr<NetworkManager> m_network;
        Netcode m_opponentNetcode;
        std::deque<MovementUpdate> m_opponentMovementUpdatesBuffer;

        GameState m_gameState = GameState::Gameplay;
};

#endif