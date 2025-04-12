#include "Game.h"

Game::Game()
{
    m_window = nullptr;
    m_renderer = nullptr;
    m_player = nullptr;
    m_opponent = nullptr;
    m_idle = nullptr;
    m_running = nullptr;
}

Game::~Game()
{
    // Destroy window
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    m_renderer = nullptr;
    m_window = nullptr;

    SDL_DestroyTexture(m_idle);
    SDL_DestroyTexture(m_running);
    m_idle = nullptr;
    m_running = nullptr;

    delete m_player;
    delete m_opponent;

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

    m_idle = TextureManager::loadTexture("assets/player-idle-spritesheet.png", m_renderer);
    m_running = TextureManager::loadTexture("assets/player-running-spritesheet.png", m_renderer);

    if (m_idle == NULL || m_running == NULL)
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
        m_player = new Player();
        m_opponent = new Player();

        m_player->setAnimationTexture(PLAYER_ANIMATION_TAG_IDLE, m_idle);
        m_opponent->setAnimationTexture(PLAYER_ANIMATION_TAG_IDLE, m_idle);

        m_player->setAnimationTexture(PLAYER_ANIMATION_TAG_RUNNING, m_running);
        m_opponent->setAnimationTexture(PLAYER_ANIMATION_TAG_RUNNING, m_running);

        m_network = std::unique_ptr<NetworkManager>(new NetworkManager(m_isHost));
        m_network->init();

        // If not host, try to connect to the server, probably want a better way
        // to do this in the future, like user presses a button to join, then
        // try to connect
        if (!m_isHost)
        {
            success = m_network->connectToServer();
        }

        if (success)
        {
            m_network->setOnOpponentMsg([this](GameMessage &msg){ handleOpponentMsg(msg); });
            m_network->start();
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
}

void Game::handleOpponentMsg(GameMessage &msg)
{
    m_opponent->setPosition(msg.posX, msg.posY);
    m_opponent->setVelocity(msg.velX, msg.velY);

    if (m_opponent->getState() != msg.state)
    {
        m_opponent->changeState(msg.state);
    }
}

void Game::sendPlayerMovement()
{
    GameMessage msg;

    msg.posX = m_player->m_position->x;
    msg.posY = m_player->m_position->y;
    msg.velX = m_player->m_velocity->x;
    msg.velY = m_player->m_velocity->y;
    msg.state = m_player->getState();

    m_network->sendPlayerMsg(msg);
}

void Game::run()
{
    bool quit = false;

    SDL_Event event;

    int lastTick = SDL_GetTicks();

    spawnPlayers();

    while (!quit)
    {
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
            m_player->input(SDLEventTranslator::translate(event));
        }

        int currentTick = SDL_GetTicks();
        int deltaTime = currentTick - lastTick;
        lastTick = currentTick;

        m_player->update(deltaTime);
        m_opponent->update(deltaTime, false); // False for don't update physics (since that comes from network)

        sendPlayerMovement();

        // Render
        SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
        SDL_RenderClear(m_renderer);

        m_player->render(m_renderer);
        m_opponent->render(m_renderer);

        SDL_RenderPresent(m_renderer);

        const int frameTime = SDL_GetTicks() - currentTick;
        if (frameTime < GAME_TICK_RATE_MS)
        {
            SDL_Delay(GAME_TICK_RATE_MS - frameTime);
        }
    }
}
