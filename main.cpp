#include <stdio.h>
#include <iostream>
#include <string>
#include <format>

#include "SystemManager.h"
#include "Components/Components.h"
#include "Systems/Systems.h"
#include "TextureManager.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "BattleScene.h"
#include "Player.h"
#include "Constants.h"

SDL_Rect createSpriteClip(int pixelsFromLeft,
                          int pixelsFromTop,
                          int entityWidth,
                          int entityHeight,
                          int row,
                          int col)
{
    // TODO: Use constant for magic 32 (original sprite scale is 32x32) and 15 (scaled up sprite, 480x480)
    SDL_Rect r;
    r.x = (pixelsFromLeft * 15) + (col * (32 * 15));
    r.y = (pixelsFromTop * 15) + (row * (32 * 15));
    r.w = entityWidth * 15;
    r.h = entityHeight * 15;

    return r;
}

class App
{
    public:
        // Initializes variables
        App();

        bool init();

        void run();

        // Deallocates memory
        ~App();

    private:
        SDL_Window* m_window;
        SDL_Renderer* m_renderer;
        BattleScene* m_scene;
        SDL_Texture* m_idle;
        SDL_Texture* m_running;
        SystemManager* m_systemManager;
};

App::App()
{
    m_window = nullptr;
    m_renderer = nullptr;
    m_scene = nullptr;
    m_idle = nullptr;
    m_running = nullptr;
    m_systemManager = nullptr;
}

bool App::init()
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

        if (m_window == NULL)
        {
            printf("Window could not be created! error=[%s]", SDL_GetError());
            success = false;
        }
        else
        {
            m_renderer = SDL_CreateRenderer(m_window,
                                           -1,
                                           SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

            if (m_renderer == NULL)
            {
                printf("Renderer could not be created! error=[%s]", SDL_GetError());
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
        }
    }

    if (success)
    {
        m_idle = TextureManager::loadTexture("assets/playerIdleSpriteSheet.png", m_renderer);
        m_running = TextureManager::loadTexture("assets/playerRunningSpriteSheet.png", m_renderer);

        Animation *idle = new Animation(8);
        idle->setTexture(m_idle);
        idle->addClip(createSpriteClip(11, 7, 9, 21, 0, 0));
        idle->addClip(createSpriteClip(11, 7, 9, 21, 0, 1));
        idle->addClip(createSpriteClip(11, 7, 9, 21, 0, 2));
        idle->addClip(createSpriteClip(11, 7, 9, 21, 1, 0));
        idle->addClip(createSpriteClip(11, 7, 9, 21, 1, 1));
        idle->addClip(createSpriteClip(11, 7, 9, 21, 1, 2));

        Animation *running = new Animation(10);
        running->setTexture(m_running);
        running->addClip(createSpriteClip(11, 7, 9, 21, 0, 0));
        running->addClip(createSpriteClip(11, 7, 9, 21, 0, 1));
        running->addClip(createSpriteClip(11, 7, 9, 21, 0, 2));
        running->addClip(createSpriteClip(11, 7, 9, 21, 1, 0));
        running->addClip(createSpriteClip(11, 7, 9, 21, 1, 1));
        running->addClip(createSpriteClip(11, 7, 9, 21, 1, 2));
        running->addClip(createSpriteClip(11, 7, 9, 21, 2, 0));
        running->addClip(createSpriteClip(11, 7, 9, 21, 2, 1));

        SpriteComponent *spriteComponent = new SpriteComponent();
        spriteComponent->addAnimation("idle", idle);
        spriteComponent->addAnimation("running", running);
        spriteComponent->setCurrentAnimation("running");

        TransformComponent *transformComponent = new TransformComponent(0, 200, 45, 105);
        VelocityComponent *velocityComponent = new VelocityComponent(0, 0, 6);

        Player *player = new Player();
        player->setSpriteComponent(spriteComponent);
        player->setTransformComponent(transformComponent);
        player->setVelocityComponent(velocityComponent);

        m_scene = new BattleScene();
        m_scene->setPlayer(player);

        RenderSystem *renderSystem = new RenderSystem(m_renderer);
        AnimationSystem *animationSystem = new AnimationSystem();
        InputSystem *inputSystem = new InputSystem();
        MoveSystem *moveSystem = new MoveSystem();
        CollisionSystem *collisionSystem = new CollisionSystem();

        renderSystem->setBattleScene(m_scene);
        animationSystem->setBattleScene(m_scene);
        inputSystem->setBattleScene(m_scene);
        moveSystem->setBattleScene(m_scene);
        collisionSystem->setBattleScene(m_scene);

        m_systemManager = new SystemManager();
        m_systemManager->addSystem("1-AnimationSystem", animationSystem);
        m_systemManager->addSystem("2-InputSystem", inputSystem);
        m_systemManager->addSystem("3-MoveSystem", moveSystem);
        m_systemManager->addSystem("4-CollisionSystem", collisionSystem);
        m_systemManager->addSystem("5-RenderSystem", renderSystem);
    }

    return success;
}

void App::run()
{
    bool quit = false;

    SDL_Event event;

    while (!quit)
    {
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
            m_systemManager->input(event);
        }

        m_systemManager->update(SDL_GetTicks());
    }
}

App::~App()
{
    // Destroy window
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    m_renderer = NULL;
    m_window = NULL;

    SDL_DestroyTexture(m_idle);
    SDL_DestroyTexture(m_running);
    m_idle = NULL;
    m_running = NULL;

    delete m_systemManager;
    delete m_scene;

    // Quit SDL subsystems
    SDL_Quit();
}

int main(int argc, char* args[])
{
    App app = App();

    if (app.init())
    {
        app.run();
    }
}
