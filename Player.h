#ifndef PLAYER_H_
#define PLAYER_H_

#include <map>

#include "SDL2/SDL.h"

#include "Constants.h"
#include "Resources.h"
#include "Input.h"
#include "Sprite.h"
#include "Transform.h"
#include "Vector2D.h"

enum PlayerAnimationTag {
    PLAYER_ANIMATION_TAG_IDLE,
    PLAYER_ANIMATION_TAG_RUNNING
};

class InputManager
{
    public:
        InputManager();

        void input(InputEvent inputEvent);
        bool isKeyPressed(Input action);

    private:
        std::unordered_map<Input, bool> m_keyState;
};

class Player;

enum class PlayerStateName
{
    None,
    Idle,
    Running
};

// Interface for all states
class PlayerState
{
    public:
        virtual ~PlayerState() = default;

        virtual void enter(Player &player) {}
        virtual void input(Player &player, InputEvent inputEvent) {}
        virtual void update(Player &player, int deltaTime) {}
        virtual void exit(Player &player) {}

        virtual PlayerStateName name() { return PlayerStateName::None; }
        virtual SDL_Texture *texture() { return nullptr; };
};

class IdleState : public PlayerState {
    public:
        void enter(Player &player) override;
        void input(Player &player, InputEvent inputEvent) override;
        void update(Player &player, int deltaTime) override;
        void exit(Player &player) override;

        PlayerStateName name() override;
        SDL_Texture *texture() override;
};

class RunningState : public PlayerState {
    public:
        void enter(Player &player) override;
        void input(Player &player, InputEvent inputEvent) override;
        void update(Player &player, int deltaTime) override;
        void exit(Player &player) override;

        PlayerStateName name() override;
        SDL_Texture *texture() override;
};

class Player
{
    public:
        static constexpr float speed = 0.4; // Pixels per ms

        Player();
        ~Player();

        void setPosition(float x, float y);
        void setVelocity(float x, float y);
        void setTransform(int width, int height);

        void input(InputEvent inputEvent);
        void update(int deltaTime);
        void render(SDL_Renderer *renderer);

        PlayerStateName getState();

    // private:
        void changeState(PlayerStateName state);
        void boundPosition();

        Vector2D<float> *m_position;
        Vector2D<float> *m_velocity;
        Transform<int> *m_transform;
        PlayerState *m_currentState;
        InputManager *m_inputManager;
        Sprite *m_sprite;
};

#endif