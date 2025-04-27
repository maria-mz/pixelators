#include "Game.h"

Game::Game()
{
    m_window = nullptr;
    m_renderer = nullptr;
}

Game::~Game()
{
    // Destroy window
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    m_renderer = nullptr;
    m_window = nullptr;

    // Quit SDL subsystems
    SDL_Quit();

    m_network->shutdown();
}

bool Game::initWindow()
{
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        LOG_ERROR("Couldn't initialize SDL: %s", SDL_GetError());
        success = false;
    }
    else
    {
        m_window = SDL_CreateWindow(Constants::WINDOW_TITLE,
                                   SDL_WINDOWPOS_UNDEFINED,
                                   SDL_WINDOWPOS_UNDEFINED,
                                   Constants::WINDOW_WIDTH,
                                   Constants::WINDOW_HEIGHT,
                                   SDL_WINDOW_SHOWN);

        if (m_window == nullptr)
        {
            LOG_ERROR("Couldn't create window: %s", SDL_GetError());
            success = false;
        }
    }

    return success;
}

bool Game::initRenderer()
{
    bool success = true;

    m_renderer = SDL_CreateRenderer(m_window,
                                    -1,
                                    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (m_renderer == nullptr)
    {
        LOG_ERROR("Couldn't create renderer: %s", SDL_GetError());
        success = false;
    }
    else
    {
        SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);

        if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
        {
            LOG_ERROR("Couldn't initialize SDL_image: %s", SDL_GetError());
            success = false;
        }
    }

    return success;
}

bool Game::initTextures()
{
    bool success = true;

    if (
        !Resources::textures.loadTexture(Constants::FILE_SPRITE_PLAYER_IDLE, m_renderer) ||
        !Resources::textures.loadTexture(Constants::FILE_SPRITE_PLAYER_RUNNING, m_renderer) ||
        !Resources::textures.loadTexture(Constants::FILE_SPRITE_PLAYER_ATTACK, m_renderer)
    )
    {
        success = false;
    }

    return success;
}

bool Game::init(bool isHost)
{
    m_isHost = isHost;

    bool success = false;

    if (initWindow())
    {
        if (initRenderer())
        {
            if (initTextures())
            {
                success = true;
            }
        }
    }

    if (success)
    {
        m_player = std::unique_ptr<Player>(new Player());
        m_opponent = std::unique_ptr<Player>(new Player());

        m_network = std::shared_ptr<NetworkManager>(new NetworkManager(m_isHost));
        m_network->init();

        // If not host, try to connect to the server, probably want a better way
        // to do this in the future, like user presses a button to join, then
        // try to connect
        if (!m_isHost)
        {
            success = m_network->connectToServer();
        }
    }

    return success;
}

void Game::spawnPlayers()
{
    m_player->setScale(5);
    m_opponent->setScale(5);

    if (m_isHost)
    {
        m_player->setPosition(PLAYER_1_SPAWN_POSITION.x, PLAYER_1_SPAWN_POSITION.y);
        m_opponent->setPosition(PLAYER_2_SPAWN_POSITION.x, PLAYER_2_SPAWN_POSITION.y);
    }
    else
    {
        m_player->setPosition(PLAYER_2_SPAWN_POSITION.x, PLAYER_2_SPAWN_POSITION.y);
        m_opponent->setPosition(PLAYER_1_SPAWN_POSITION.x, PLAYER_1_SPAWN_POSITION.y);
    }

    // Set initial net opponent data
    m_opponentNetcode.setNetPlayerData({
        {m_opponent->m_position.x, m_opponent->m_position.y},
        {m_opponent->m_velocity.x, m_opponent->m_velocity.y}
    });
}

void Game::handleOpponentNetMsgs()
{
    GameMessage opponentMsg;

    while (m_network->receiveOpponentMsg(opponentMsg))
    {
        // Right now all game messages are update messages
        m_opponentUpdatesBuffer.push_back(opponentMsg);
    }
}

void Game::renderPlayer(std::shared_ptr<Player> player)
{
    #ifdef DEBUG_MODE
    player->render(m_renderer, true, true, true);
    #else
    player->render(m_renderer);
    #endif
}

void Game::render()
{
    SDL_SetRenderDrawColor(m_renderer, 67, 67, 67, 255);
    SDL_RenderClear(m_renderer);

    if (
        m_opponent->getState() == PlayerState::Attack &&
        m_player->getState() != PlayerState::Attack
    )
    {
        renderPlayer(m_player);
        renderPlayer(m_opponent);
    }
    else
    {
        renderPlayer(m_opponent);
        renderPlayer(m_player);
    }

    SDL_RenderPresent(m_renderer);
}

void Game::updateOpponent(int deltaTime)
{
    GameMessage updateMsg;

    while (m_opponentUpdatesBuffer.size() > MIN_OPPONENT_LAG_FRAMES)
    {
        updateMsg = m_opponentUpdatesBuffer.front();
        m_opponentUpdatesBuffer.pop_front();

        if (updateMsg.inputEvent != InputEvent::None)
        {
            m_opponent->input(updateMsg.inputEvent);
        }

        m_opponentNetcode.updateNetState(updateMsg);
    }

    m_opponent->update(deltaTime);
    m_opponentNetcode.syncPlayerWithNetState(*m_opponent);
}

void Game::run()
{
    spawnPlayers();

    bool quit = false;

    FrameTimer frameTimer(GAME_TICK_RATE_MS);

    GameMessage playerUpdateMsg;

    SDL_Event event;
    InputEvent playerInputEvent;

    while (!quit)
    {
        frameTimer.startFrame();

        playerInputEvent = InputEvent::None;

        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
            playerInputEvent = SDLEventTranslator::translate(event);
            if (playerInputEvent != InputEvent::None)
            {
                m_player->input(playerInputEvent);
                break; // Input only one player event per frame
            }
        }

        handleOpponentNetMsgs();

        m_player->update(frameTimer.getDeltaTime());
        updateOpponent(frameTimer.getDeltaTime());

        if (m_player->isHitBy(*m_opponent))
        {
            printf("Opponent hit player!!\n");
        }

        playerUpdateMsg.posX = m_player->m_position.x;
        playerUpdateMsg.posY = m_player->m_position.y;
        playerUpdateMsg.velX = m_player->m_velocity.x;
        playerUpdateMsg.velY = m_player->m_velocity.y;
        playerUpdateMsg.inputEvent = playerInputEvent;

        m_network->sendPlayerMsg(playerUpdateMsg);

        render();

        frameTimer.endFrame();
    }
}
