#include "Player.h"

InputManager::InputManager()
{
    m_keyState[ACTION_MOVE_LEFT] = false;
    m_keyState[ACTION_MOVE_RIGHT] = false;
}

void InputManager::input(Event event)
{
    switch (event)
    {
        case EVENT_MOVE_LEFT_PRESSED: m_keyState[ACTION_MOVE_LEFT] = true; break;
        case EVENT_MOVE_RIGHT_PRESSED: m_keyState[ACTION_MOVE_RIGHT] = true; break;
        case EVENT_MOVE_LEFT_RELEASED: m_keyState[ACTION_MOVE_LEFT] = false; break;
        case EVENT_MOVE_RIGHT_RELEASED: m_keyState[ACTION_MOVE_RIGHT] = false; break;
        default: break;
    }
}

bool InputManager::isKeyPressed(Action action)
{
    return m_keyState[action];
}


void IdleState::enter(Player &player)
{
    player.m_sprite->getAnimator()->setCurrentAnimation(PLAYER_ANIMATION_TAG_IDLE);
}

void IdleState::input(Player &player, Event event)
{
    switch (event)
    {
        case EVENT_MOVE_LEFT_PRESSED: player.m_velocity->x -= player.speed; break;
        case EVENT_MOVE_RIGHT_PRESSED:  player.m_velocity->x += player.speed; break;
        default: break;
    }

    if (player.m_inputManager->isKeyPressed(ACTION_MOVE_LEFT) ||
        player.m_inputManager->isKeyPressed(ACTION_MOVE_RIGHT))
    {
        player.changeState(new RunningState());
    }
}

void IdleState::update(Player &player, int deltaTime)
{

}

void IdleState::exit(Player &player)
{

}

void RunningState::enter(Player &player)
{
    player.m_sprite->getAnimator()->setCurrentAnimation(PLAYER_ANIMATION_TAG_RUNNING);
}

void RunningState::input(Player &player, Event event)
{
    switch (event)
    {
        case EVENT_MOVE_LEFT_PRESSED: player.m_velocity->x -= player.speed; break;
        case EVENT_MOVE_RIGHT_PRESSED:  player.m_velocity->x += player.speed; break;
        case EVENT_MOVE_LEFT_RELEASED: player.m_velocity->x += player.speed; break;
        case EVENT_MOVE_RIGHT_RELEASED: player.m_velocity->x -= player.speed; break;
        default: break;
    }

    if (!player.m_inputManager->isKeyPressed(ACTION_MOVE_LEFT) &&
        !player.m_inputManager->isKeyPressed(ACTION_MOVE_RIGHT))
    {
        player.changeState(new IdleState());
    }
}

void RunningState::update(Player &player, int deltaTime)
{
    player.m_position->x += deltaTime * player.m_velocity->x;

    Animation *animation = player.m_sprite->getAnimator()->getAnimation(PLAYER_ANIMATION_TAG_RUNNING);

    if (player.m_velocity->x > 0)
    {
        animation->setFlip(SDL_FLIP_NONE);
    }
    else
    {
        animation->setFlip(SDL_FLIP_HORIZONTAL);
    }
}

void RunningState::exit(Player &player)
{

}


Player::Player()
{
    m_inputManager = new InputManager();
    m_currentState = new IdleState();
    m_transform = new Transform<int>;
    m_position = new Vector2D<float>;
    m_velocity = new Vector2D<float>;

    // Set up sprite
    auto createClip = makeSpriteClipper(32, 32, 15);

    std::vector<SDL_Rect> idleAnimationFrames = {
        createClip(11, 7, 9, 21, 0, 0),
        createClip(11, 7, 9, 21, 0, 1),
        createClip(11, 7, 9, 21, 0, 2),
        createClip(11, 7, 9, 21, 1, 0),
        createClip(11, 7, 9, 21, 1, 1),
        createClip(11, 7, 9, 21, 1, 2)
    };

    std::vector<SDL_Rect> runningAnimationFrames = {
        createClip(11, 7, 9, 21, 0, 0),
        createClip(11, 7, 9, 21, 0, 1),
        createClip(11, 7, 9, 21, 0, 2),
        createClip(11, 7, 9, 21, 1, 0),
        createClip(11, 7, 9, 21, 1, 1),
        createClip(11, 7, 9, 21, 1, 2),
        createClip(11, 7, 9, 21, 2, 0),
        createClip(11, 7, 9, 21, 2, 1)
    };

    Animation *idleAnimation = new Animation(8);
    Animation *runningAnimation = new Animation(10);

    idleAnimation->setFrames(idleAnimationFrames);
    runningAnimation->setFrames(runningAnimationFrames);

    Animator *animator = new Animator();
    animator->addAnimation(PLAYER_ANIMATION_TAG_IDLE, idleAnimation);
    animator->addAnimation(PLAYER_ANIMATION_TAG_RUNNING, runningAnimation);

    m_sprite = new Sprite();
    m_sprite->setAnimator(animator);

    m_currentState->enter(*this);
}

Player::~Player()
{
    m_currentState->exit(*this);

    delete m_inputManager;
    delete m_currentState;
    delete m_transform;
    delete m_position;
    delete m_velocity;
    delete m_sprite;
}

void Player::setPosition(float x, float y)
{
    m_position->x = x;
    m_position->y = y;
}

void Player::setVelocity(float x, float y)
{
    m_velocity->x = x;
    m_velocity->y = y;
}

void Player::setTransform(int width, int height)
{
    m_transform->width = width;
    m_transform->height = height;
}

void Player::setAnimationTexture(int animationTag, SDL_Texture *texture)
{
    m_sprite->getAnimator()->getAnimation(animationTag)->setTexture(texture);
}

void Player::input(Event event)
{
    m_inputManager->input(event);
    m_currentState->input(*this, event);
}

void Player::update(int deltaTime)
{
    m_currentState->update(*this, deltaTime);
    m_sprite->getAnimator()->updateCurrentAnimation(deltaTime);
    boundPosition();
}

void Player::changeState(PlayerState *newState)
{
    m_currentState->exit(*this);
    delete m_currentState;
    m_currentState = newState;
    m_currentState->enter(*this);
}

void Player::boundPosition()
{
    if (m_position->x < 0)
    {
        m_position->x = 0;
    }
    else if (m_position->x > (PX_WINDOW_WIDTH - m_transform->width))
    {
        m_position->x = PX_WINDOW_WIDTH - m_transform->width;
    }
}
