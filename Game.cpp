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

    if (TTF_Init() < 0)
    {
        LOG_ERROR("Couldn't initialize SDL TTF: %s", SDL_GetError());
        success = false;
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

        Resources::renderer = m_renderer;

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
        !Resources::textures.loadTexture(Constants::FILE_SPRITE_PLAYER_IDLE_RED, m_renderer) ||
        !Resources::textures.loadTexture(Constants::FILE_SPRITE_PLAYER_IDLE_BLUE, m_renderer) ||
        !Resources::textures.loadTexture(Constants::FILE_SPRITE_PLAYER_RUN_RED, m_renderer) ||
        !Resources::textures.loadTexture(Constants::FILE_SPRITE_PLAYER_RUN_BLUE, m_renderer) ||
        !Resources::textures.loadTexture(Constants::FILE_SPRITE_PLAYER_ATTACK_RED, m_renderer) ||
        !Resources::textures.loadTexture(Constants::FILE_SPRITE_PLAYER_ATTACK_BLUE, m_renderer) ||
        !Resources::textures.loadTexture(Constants::FILE_SPRITE_PLAYER_KNOCKBACK_RED, m_renderer) ||
        !Resources::textures.loadTexture(Constants::FILE_SPRITE_PLAYER_KNOCKBACK_BLUE, m_renderer) ||
        !Resources::textures.loadTexture(Constants::FILE_SPRITE_PLAYER_BLOCK_RED, m_renderer) ||
        !Resources::textures.loadTexture(Constants::FILE_SPRITE_PLAYER_BLOCK_BLUE, m_renderer) ||
        !Resources::textures.loadTexture(Constants::FILE_SPRITE_HEALTH_BAR_OK, m_renderer) ||
        !Resources::textures.loadTexture(Constants::FILE_SPRITE_HEALTH_BAR_LOW, m_renderer) ||
        !Resources::textures.loadTexture(Constants::FILE_CURSOR, m_renderer)
    )
    {
        success = false;
    }

    if (
        !Resources::fonts.loadFont(Constants::FILE_FONT_MAIN, 8, m_renderer) ||
        !Resources::fonts.loadFont(Constants::FILE_FONT_MAIN, 12, m_renderer) ||
        !Resources::fonts.loadFont(Constants::FILE_FONT_MAIN, 18, m_renderer)
    )
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
        m_player = std::unique_ptr<Player>(new Player());
        m_opponent = std::unique_ptr<Player>(new Player());

        m_network = std::shared_ptr<NetworkManager>(new NetworkManager());

        m_menuUI.init();
        m_menuUI.setOnHostGameButtonClick([this]() {
            m_isHost = true;
            m_network->setIsHost(true);
            m_network->startServer();
            m_gameState = GameState::Gameplay;
            setPlayerTexturesRed(*m_player);
            setPlayerTexturesBlue(*m_opponent);
            spawnPlayers();
        });

        m_menuUI.setOnJoinGameButtonClick([this]() {
            m_isHost = false;
            m_network->setIsHost(false);
            bool isConnected = m_network->connectToServer(m_menuUI.getIPAddressFromTextField());
            if (isConnected)
            {
                m_gameState = GameState::Gameplay;
                setPlayerTexturesBlue(*m_player);
                setPlayerTexturesRed(*m_opponent);
                spawnPlayers();
            }
            else
            {
                LOG_ERROR("Failed to join game");
            }
        });

        m_gameplayUI.init();
        m_gameplayUI.setLowHP(LOW_HP);
        m_gameplayUI.setMaxHealth(Player::MAX_HEALTH);

        SDL_Surface* surface = IMG_Load(Constants::FILE_CURSOR);
        SDL_Cursor* cursor = SDL_CreateColorCursor(surface, 12, 8);
        SDL_SetCursor(cursor);
        SDL_FreeSurface(surface);
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
        if (opponentMsg.type == GameMessageType::MovementUpdate)
        {
            m_opponentMovementUpdatesBuffer.push_back(opponentMsg.data.movementUpdate);
        }
        else if (opponentMsg.type == GameMessageType::ServerRegisteredHit)
        {
            HitRegistered hitRegistered = opponentMsg.data.hitRegistered;

            if (hitRegistered.hitPlayerID == PLAYER_1_ID)
            {
                m_opponent->registerHitTaken(*m_player);
            }
            else if (hitRegistered.hitPlayerID == PLAYER_2_ID)
            {
                m_player->registerHitTaken(*m_opponent);
            }
        }
    }
}

void Game::sendPlayerMovementUpdate(const MovementUpdate &movementUpdate)
{
    GameMessage msg;
    msg.type = GameMessageType::MovementUpdate;
    msg.data.movementUpdate = movementUpdate;

    m_network->sendPlayerMsg(msg);
}

void Game::sendHitRegistered(const HitRegistered &hitRegistered)
{
    if (!m_isHost)
    {
        throw std::logic_error("Only the host can send ServerRegisteredHit messages");
    }

    GameMessage msg;
    msg.type = GameMessageType::ServerRegisteredHit;
    msg.data.hitRegistered = hitRegistered;

    m_network->sendPlayerMsg(msg);
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

    switch (m_gameState)
    {
        case GameState::Menu:
        {
            renderMenu();
            break;
        }
        case GameState::Gameplay:
        {
            renderGameplay();
            break;
        }
    }

    SDL_RenderPresent(m_renderer);
}

void Game::renderMenu()
{
    m_menuUI.render();
}

void Game::renderGameplay()
{
    SDL_Rect groundRect = {0, 360, Constants::WINDOW_WIDTH, 120};
    SDL_SetRenderDrawColor(Resources::renderer, 50, 50, 50, 255);
    SDL_RenderFillRect(Resources::renderer, &groundRect);

    if (m_opponent->getState() == PlayerState::Attack &&
        m_player->getState() != PlayerState::Attack)
    {
        renderPlayer(m_player);
        renderPlayer(m_opponent);
    }
    else
    {
        renderPlayer(m_opponent);
        renderPlayer(m_player);
    }

    if (m_isHost)
    {
        m_gameplayUI.render(m_player->getHealth(), m_opponent->getHealth());
    }
    else
    {
        m_gameplayUI.render(m_opponent->getHealth(), m_player->getHealth());
    }
}

void Game::updateOpponent(int deltaTime)
{
    MovementUpdate movementUpdate;

    while (m_opponentMovementUpdatesBuffer.size() > MIN_OPPONENT_LAG_FRAMES)
    {
        movementUpdate = m_opponentMovementUpdatesBuffer.front();
        m_opponentMovementUpdatesBuffer.pop_front();

        if (movementUpdate.inputEvent != InputEvent::None)
        {
            m_opponent->input(movementUpdate.inputEvent);
        }

        if (movementUpdate.direction != m_opponent->getDirection())
        {
            m_opponent->updateDirection(movementUpdate.direction);
        }

        m_opponentNetcode.updateNetState(movementUpdate);
    }

    m_opponent->update(deltaTime);
    m_opponentNetcode.syncPlayerWithNetState(*m_opponent);
}

void Game::handleEvent(const SDL_Event &event)
{
    switch (m_gameState)
    {
        case GameState::Gameplay:
        {
            InputEvent playerInputEvent = SDLEventTranslator::translate(event);
            if (playerInputEvent != InputEvent::None)
            {
                m_playerInputEvent = playerInputEvent;
                m_player->input(m_playerInputEvent);
            }

            break;
        }
        case GameState::Menu:
        {
            m_menuUI.handleEvent(event);
            break;
        }
    }
}

void Game::update(const int deltaTime)
{
    switch (m_gameState)
    {
        case GameState::Gameplay:
        {
            handleOpponentNetMsgs();

            m_player->update(deltaTime);
            updateOpponent(deltaTime);

            bool isPlayerHit = m_player->isHitBy(*m_opponent);
            bool isOpponentHit = m_opponent->isHitBy(*m_player);

            if (m_isHost)
            {
                HitRegistered hitRegistered;

                if (isPlayerHit)
                {
                    m_player->registerHitTaken(*m_opponent);

                    hitRegistered.hitPlayerID = PLAYER_1_ID;
                    sendHitRegistered(hitRegistered);
                }
                if (isOpponentHit)
                {
                    m_opponent->registerHitTaken(*m_player);

                    hitRegistered.hitPlayerID = PLAYER_2_ID;
                    sendHitRegistered(hitRegistered);
                }
            }

            MovementUpdate playerMovementUpdate;
            playerMovementUpdate.posX = m_player->m_position.x;
            playerMovementUpdate.posY = m_player->m_position.y;
            playerMovementUpdate.velX = m_player->m_velocity.x;
            playerMovementUpdate.velY = m_player->m_velocity.y;
            playerMovementUpdate.inputEvent = m_playerInputEvent;
            playerMovementUpdate.direction = m_player->getDirection();

            sendPlayerMovementUpdate(playerMovementUpdate);

            break;
        }
    }
}

void Game::run()
{
    spawnPlayers();

    bool quit = false;
    SDL_Event event;
    FrameTimer frameTimer(GAME_TICK_RATE_MS);

    InputEvent playerInputEvent;

    while (!quit)
    {
        frameTimer.startFrame();

        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }

            handleEvent(event);
            break;
        }

        update(frameTimer.getDeltaTime());
        render();

        m_playerInputEvent = InputEvent::None;
        frameTimer.endFrame();
    }
}
