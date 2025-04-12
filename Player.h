#ifndef PLAYER_H_
#define PLAYER_H_

#include "SDL2/SDL.h"

#include "Constants.h"
#include "Event.h"
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

        void input(Event event);
        bool isKeyPressed(Action action);

    private:
        bool m_keyState[ACTION_COUNT];
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
        virtual void input(Player &player, Event event) {}
        virtual void update(Player &player, int deltaTime, bool updatePhysics = true) {}
        virtual void exit(Player &player) {}

        virtual PlayerStateName name() { return PlayerStateName::None; }
};

class IdleState : public PlayerState {
    public:
        void enter(Player &player) override;
        void input(Player &player, Event event) override;
        void update(Player &player, int deltaTime, bool updatePhysics = true) override;
        void exit(Player &player) override;

        PlayerStateName name() override { return PlayerStateName::Idle; }
};

class RunningState : public PlayerState {
    public:
        void enter(Player &player) override;
        void input(Player &player, Event event) override;
        void update(Player &player, int deltaTime, bool updatePhysics = true) override;
        void exit(Player &player) override;

        PlayerStateName name() override { return PlayerStateName::Running; }
};

class Player
{
    public:
        static constexpr float speed = 0.4; // Pixels per ms

        Player();
        ~Player();

        void setAnimationTexture(int animationTag, SDL_Texture *texture);

        void setPosition(float x, float y);
        void setVelocity(float x, float y);
        void setTransform(int width, int height);

        void input(Event event);
        void update(int deltaTime, bool updatePhysics = true);
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