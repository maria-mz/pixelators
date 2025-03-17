#include <stdio.h>
#include <iostream>
#include <string>
#include <format>

#include "ECS/ECS.h"
#include "ECS/Components.h"
#include "ECS/Systems.h"
#include "TextureManager.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

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
        Entity* m_player;
        SDL_Texture* m_idle;
        SystemManager* m_systemManager;
};

App::App()
{
    m_window = nullptr;
    m_renderer = nullptr;
    m_player = nullptr;
    m_idle = nullptr;
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

        SpriteComponent *spriteComponent = new SpriteComponent(8);
        spriteComponent->setTexture(m_idle);
        spriteComponent->addClip({0, 0, 480, 480});
        spriteComponent->addClip({480, 0, 480, 480});
        spriteComponent->addClip({480 * 2, 0, 480, 480});
        spriteComponent->addClip({0, 480, 480, 480});
        spriteComponent->addClip({480, 480, 480, 480});
        spriteComponent->addClip({480 * 2, 480, 480, 480});

        TransformComponent *transformComponent = new TransformComponent(0, 0, 128, 128);

        m_player = new Entity();
        m_player->addComponent(spriteComponent);
        m_player->addComponent(transformComponent);

        RenderSystem *renderSystem = new RenderSystem(m_renderer);
        renderSystem->addEntity(m_player);

        AnimationSystem *animationSystem = new AnimationSystem();
        animationSystem->addEntity(m_player);

        m_systemManager = new SystemManager();
        m_systemManager->addSystem("1-AnimationSystem", animationSystem);
        m_systemManager->addSystem("2-RenderSystem", renderSystem);
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
    m_idle = NULL;

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
