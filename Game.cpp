#include "Game.h"

static Vector2D PLAYER_1_SPAWN_POSITION(180, 200);
static Vector2D PLAYER_2_SPAWN_POSITION(540, 200);

Game::Game()
{
    m_window = nullptr;
    m_renderer = nullptr;
    m_player = nullptr;
    m_opponent = nullptr;
    m_idle = nullptr;
    m_running = nullptr;
    m_gameTickRate = 16; // ~16 ms per tick (~60 updates per second)
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
            int retries = 0;
            while (retries < 3 && !m_network->isConnectedToServer())
            {
                printf("Trying to connect to server...\n");
                m_network->connectToServer();
                retries++;

                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }

            success = m_network->isConnectedToServer();
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

void Game::run()
{
    bool quit = false;

    SDL_Event event;

    int lastTick = SDL_GetTicks();

    spawnPlayers();

    int opponentEvents = 0;
    Event opponentEvent = EVENT_NONE;

    while (!quit)
    {
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
            m_player->input(SDLEventTranslator::translate(event));
            m_network->sendPlayerInputEvent(SDLEventTranslator::translate(event));
        }

        // Input opponent events
        while (opponentEvents < MAX_OPPONENT_EVENTS_PER_TICK)
        {
            opponentEvent = m_network->receiveOpponentInputEvent();

            if (opponentEvent == EVENT_NONE)
            {
                break;
            }
            else
            {
                m_opponent->input(opponentEvent);
            }
        }
        opponentEvents = 0; // reset

        int currentTick = SDL_GetTicks();
        int deltaTime = currentTick - lastTick;
        lastTick = currentTick;

        m_player->update(deltaTime);
        m_opponent->update(deltaTime);

        // Render
        SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
        SDL_RenderClear(m_renderer);

        m_player->render(m_renderer);
        m_opponent->render(m_renderer);

        SDL_RenderPresent(m_renderer);

        const int frameTime = SDL_GetTicks() - currentTick;
        if (frameTime < m_gameTickRate)
        {
            SDL_Delay(m_gameTickRate - frameTime);
        }
    }
}
