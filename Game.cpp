#include "Game.h"

Game::Game()
{
    m_window = nullptr;
    m_renderer = nullptr;
    m_player = nullptr;
    m_idle = nullptr;
    m_running = nullptr;
    m_renderSystem = nullptr;
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

    delete m_renderSystem;
    delete m_player;

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

bool Game::init()
{
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
        m_player->setPosition(0, 200);
        m_player->setTransform(45, 105);
        m_player->setAnimationTexture(PLAYER_ANIMATION_TAG_IDLE, m_idle);
        m_player->setAnimationTexture(PLAYER_ANIMATION_TAG_RUNNING, m_running);

        m_renderSystem = new RenderSystem(m_renderer, m_player);
    }

    return success;
}

void Game::run()
{
    bool quit = false;

    SDL_Event event;

    int lastTick = SDL_GetTicks();

    while (!quit)
    {
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
            // TODO: Add player event to event bus for sending over the network
            m_player->input(SDLEventTranslator::translate(event));
        }

        // TODO: Read opponent events from network and call its input method

        // TODO: Send player's events over the network

        int currentTick = SDL_GetTicks();
        int deltaTime = currentTick - lastTick;
        lastTick = currentTick;

        // TODO: update opponent too
        m_player->update(deltaTime);
        m_renderSystem->update();

        const int frameTime = SDL_GetTicks() - currentTick;
        if (frameTime < m_gameTickRate)
        {
            SDL_Delay(m_gameTickRate - frameTime);
        }
    }
}
