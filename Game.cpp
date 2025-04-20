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
        printf("SDL failed to initialize! error=[%s]", SDL_GetError());
        success = false;
    }
    else
    {
        m_window = SDL_CreateWindow(PX_WINDOW_TITLE,
                                   SDL_WINDOWPOS_UNDEFINED,
                                   SDL_WINDOWPOS_UNDEFINED,
                                   PX_WINDOW_WIDTH,
                                   PX_WINDOW_HEIGHT,
                                   SDL_WINDOW_SHOWN);

        if (m_window == nullptr)
        {
            printf("Window could not be created! error=[%s]", SDL_GetError());
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
        printf("Renderer could not be created! error=[%s]", SDL_GetError());
        success = false;
    }
    else
    {
        SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);

        if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
        {
            printf("SDL_image failed to initialize! error=[%s]", SDL_GetError());
            success = false;
        }
    }

    return success;
}

bool Game::initTextures()
{
    bool success = true;

    if (
        !Resources::textures.loadTexture("assets/player-idle-spritesheet.png", m_renderer) ||
        !Resources::textures.loadTexture("assets/player-running-spritesheet.png", m_renderer)
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
    m_player->setTransform(45, 105);
    m_opponent->setTransform(45, 105);

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
        {m_opponent->m_position->x, m_opponent->m_position->y},
        {m_opponent->m_velocity->x, m_opponent->m_velocity->y}
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

void Game::render()
{
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
    SDL_RenderClear(m_renderer);

    m_player->render(m_renderer);
    m_opponent->render(m_renderer);

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

        playerUpdateMsg.posX = m_player->m_position->x;
        playerUpdateMsg.posY = m_player->m_position->y;
        playerUpdateMsg.velX = m_player->m_velocity->x;
        playerUpdateMsg.velY = m_player->m_velocity->y;
        playerUpdateMsg.inputEvent = playerInputEvent;

        m_network->sendPlayerMsg(playerUpdateMsg);

        render();

        frameTimer.endFrame();
    }
}
