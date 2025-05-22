#ifndef PLAYER_H_
#define PLAYER_H_

#include <map>

#include "SDL2/SDL.h"

#include "Constants.h"
#include "Resources.h"
#include "Input.h"
#include "Sprite.h"
#include "Utils/Vector2D.h"

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

enum class PlayerState
{
    Idle,
    Run,
    Attack,
    Knockback,
    Block
};

// Interface for all states
class PlayerStateInterface
{
    public:
        virtual ~PlayerStateInterface() = default;

        virtual void enter(Player &player) = 0;
        virtual void input(Player &player, InputEvent inputEvent) = 0;
        virtual void update(Player &player, int deltaTime) = 0;
        virtual void exit(Player &player) = 0;

        virtual SDL_Texture *getTexture() = 0;
        virtual std::shared_ptr<Animation> getAnimation(const Player &player) = 0;
};

class PlayerStateIdle : public PlayerStateInterface {
    public:
        void enter(Player &player) override;
        void input(Player &player, InputEvent inputEvent) override;
        void update(Player &player, int deltaTime) override;
        void exit(Player &player) override;

        SDL_Texture *getTexture() override;
        std::shared_ptr<Animation> getAnimation(const Player &player) override;
};

class PlayerStateRun : public PlayerStateInterface {
    public:
        void enter(Player &player) override;
        void input(Player &player, InputEvent inputEvent) override;
        void update(Player &player, int deltaTime) override;
        void exit(Player &player) override;

        SDL_Texture *getTexture() override;
        std::shared_ptr<Animation> getAnimation(const Player &player) override;
};

class PlayerStateAttack : public PlayerStateInterface {
    public:
        void enter(Player &player) override;
        void input(Player &player, InputEvent inputEvent) override;
        void update(Player &player, int deltaTime) override;
        void exit(Player &player) override;

        SDL_Texture *getTexture() override;
        std::shared_ptr<Animation> getAnimation(const Player &player) override;
};

class PlayerStateKnockback : public PlayerStateInterface {
    public:
        void enter(Player &player) override;
        void input(Player &player, InputEvent inputEvent) override;
        void update(Player &player, int deltaTime) override;
        void exit(Player &player) override;

        SDL_Texture *getTexture() override;
        std::shared_ptr<Animation> getAnimation(const Player &player) override;
};

class PlayerStateBlock : public PlayerStateInterface {
    public:
        void enter(Player &player) override;
        void input(Player &player, InputEvent inputEvent) override;
        void update(Player &player, int deltaTime) override;
        void exit(Player &player) override;

        SDL_Texture *getTexture() override;
        std::shared_ptr<Animation> getAnimation(const Player &player) override;
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
        static constexpr float SPEED = 0.35; // Pixels per ms
        static constexpr int MAX_HEALTH = 100;
        static constexpr int HIT_DMG = 10;

        Player();
        ~Player();

        void setPosition(float x, float y);
        void setVelocity(float x, float y);
        void setScale(int scale);

        int getHealth();

        void input(InputEvent inputEvent);
        void update(int deltaTime);
        void render(
            SDL_Renderer *renderer,
            bool drawBoundingBox = false,
            bool drawHitBox = false,
            bool drawHurtBox = false
        ) const;

        PlayerState getState() const;
        Direction getDirection() const;

        void updateDirection(Direction direction);

        bool isHitBy(const Player &opponent) const;
        bool maybeRegisterHit(const Player &opponent);

    // private:
        AnimationManager<PlayerState> makeAnimationManager() const;

        void changeState(PlayerState state);
        void boundPosition();

        SDL_Rect getBoundingBox() const;
        SDL_Rect getHitBox() const;
        SDL_Rect getHurtBox() const;

        SDL_Rect transformBoxToRenderSpace(SDL_Rect box) const;
        SDL_Rect transformPlayerToRenderSpace() const;

        void renderBox(SDL_Renderer *renderer, SDL_Rect box) const;
        void renderPlayer(SDL_Renderer *renderer) const;

        Vector2D<float> m_position;
        Vector2D<float> m_velocity;
        Transform m_transform;
        Direction m_direction;

        int m_health;

        bool m_isImmune;

        InputManager m_inputManager;
        AnimationManager<PlayerState> m_animationManager;

        PlayerState m_currentState;
        PlayerStateInterface *m_stateObject;
};

#endif