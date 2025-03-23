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
#include "Player.h"

constexpr const char* PX_WINDOW_TITLE = "Pixelators";
constexpr int PX_WINDOW_WIDTH = 720;
constexpr int PX_WINDOW_HEIGHT = 480;

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
        Player* m_player;
        SDL_Texture* m_idle;
        SDL_Texture* m_running;
        SystemManager* m_systemManager;
};

App::App()
{
    m_window = nullptr;
    m_renderer = nullptr;
    m_player = nullptr;
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
        idle->addClip({0, 0, 480, 480});
        idle->addClip({480, 0, 480, 480});
        idle->addClip({480 * 2, 0, 480, 480});
        idle->addClip({0, 480, 480, 480});
        idle->addClip({480, 480, 480, 480});
        idle->addClip({480 * 2, 480, 480, 480});

        Animation *running = new Animation(10);
        running->setTexture(m_running);
        running->addClip({0, 0, 480, 480});
        running->addClip({480, 0, 480, 480});
        running->addClip({480 * 2, 0, 480, 480});
        running->addClip({0, 480, 480, 480});
        running->addClip({480, 480, 480, 480});
        running->addClip({480 * 2, 480, 480, 480});
        running->addClip({0, 480 * 2, 480, 480});
        running->addClip({480, 480 * 2, 480, 480});

        SpriteComponent *spriteComponent = new SpriteComponent();
        spriteComponent->addAnimation("idle", idle);
        spriteComponent->addAnimation("running", running);
        spriteComponent->setCurrentAnimation("running");

        TransformComponent *transformComponent = new TransformComponent(0, 0, 128, 128);
        VelocityComponent *velocityComponent = new VelocityComponent(0, 0, 6);

        m_player = new Player();
        m_player->setSpriteComponent(spriteComponent);
        m_player->setTransformComponent(transformComponent);
        m_player->setVelocityComponent(velocityComponent);

        RenderSystem *renderSystem = new RenderSystem(m_renderer);
        AnimationSystem *animationSystem = new AnimationSystem();
        InputSystem *inputSystem = new InputSystem();
        MoveSystem *moveSystem = new MoveSystem();

        renderSystem->setPlayer(m_player);
        animationSystem->setPlayer(m_player);
        inputSystem->setPlayer(m_player);
        moveSystem->setPlayer(m_player);

        m_systemManager = new SystemManager();
        m_systemManager->addSystem("1-AnimationSystem", animationSystem);
        m_systemManager->addSystem("2-InputSystem", inputSystem);
        m_systemManager->addSystem("3-MoveSystem", moveSystem);
        m_systemManager->addSystem("4-RenderSystem", renderSystem);
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
    delete m_player;

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
