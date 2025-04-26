#ifndef PLAYER_H_
#define PLAYER_H_

#include <map>

#include "SDL2/SDL.h"

#include "Constants.h"
#include "Resources.h"
#include "Input.h"
#include "Sprite.h"
#include "Utils/Vector2D.h"

enum PlayerAnimationTag {
    PLAYER_ANIMATION_TAG_IDLE,
    PLAYER_ANIMATION_TAG_RUNNING,
    PLAYER_ANIMATION_TAG_ATTACK
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
    Running,
    Attack
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

class AttackState : public PlayerState {
    public:
        void enter(Player &player) override;
        void input(Player &player, InputEvent inputEvent) override;
        void update(Player &player, int deltaTime) override;
        void exit(Player &player) override;

        PlayerStateName name() override;
        SDL_Texture *texture() override;
};

struct Transform
{
    float width;
    float height;
    float scale;
};

enum class Direction
{
    Left,
    Right
};

class Player
{
    public:
        static constexpr float speed = 0.35; // Pixels per ms

        Player();
        ~Player();

        void setPosition(float x, float y);
        void setVelocity(float x, float y);
        void setScale(int scale);

        void input(InputEvent inputEvent);
        void update(int deltaTime);
        void render(
            SDL_Renderer *renderer,
            bool drawBoundingBox = false,
            bool drawHitBox = false,
            bool drawHurtBox = false
        );

        PlayerStateName getState();

        void updateDirection(Direction direction);

    // private:
        Animator *makeAnimator();

        void changeState(PlayerStateName state);
        void boundPosition();

        SDL_Rect getBoundingBox();
        SDL_Rect getHitBox();
        SDL_Rect getHurtBox();

        SDL_Rect transformBoxToRenderSpace(SDL_Rect box);

        void renderBox(SDL_Renderer *renderer, SDL_Rect box);
        void renderPlayer(SDL_Renderer *renderer);

        Vector2D<float> m_position;
        Vector2D<float> m_velocity;
        Transform m_transform;

        PlayerState *m_currentState;
        InputManager m_inputManager;
        Sprite m_sprite;

        Direction m_direction;
};

#endif