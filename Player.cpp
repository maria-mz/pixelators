#include "Player.h"

InputManager::InputManager()
{
    m_keyState = {
        {Input::MoveLeft, false},
        {Input::MoveRight, false}
    };
}

void InputManager::input(InputEvent inputEvent)
{
    switch (inputEvent)
    {
        case InputEvent::MoveLeft_Pressed: m_keyState[Input::MoveLeft] = true; break;
        case InputEvent::MoveRight_Pressed: m_keyState[Input::MoveRight] = true; break;
        case InputEvent::MoveLeft_Released: m_keyState[Input::MoveLeft] = false; break;
        case InputEvent::MoveRight_Released: m_keyState[Input::MoveRight] = false; break;
        default: break;
    }
}

bool InputManager::isKeyPressed(Input action)
{
    return m_keyState[action];
}


void IdleState::enter(Player &player)
{
    player.m_sprite->getAnimator()->setCurrentAnimation(PLAYER_ANIMATION_TAG_IDLE);
}

void IdleState::input(Player &player, InputEvent inputEvent)
{
    switch (inputEvent)
    {
        case InputEvent::MoveLeft_Pressed: player.m_velocity.x -= player.speed; break;
        case InputEvent::MoveRight_Pressed:  player.m_velocity.x += player.speed; break;
        default: break;
    }

    if (player.m_inputManager->isKeyPressed(Input::MoveLeft) ||
        player.m_inputManager->isKeyPressed(Input::MoveRight))
    {
        player.changeState(PlayerStateName::Running);
    }
}

void IdleState::update(Player &player, int deltaTime)
{

}

void IdleState::exit(Player &player)
{

}

PlayerStateName IdleState::name()
{
    return PlayerStateName::Idle;
}

SDL_Texture *IdleState::texture()
{
    return Resources::textures.getTexture(Constants::FILE_SPRITE_PLAYER_IDLE);
};

void RunningState::enter(Player &player)
{
    player.m_sprite->getAnimator()->setCurrentAnimation(PLAYER_ANIMATION_TAG_RUNNING);
}

void RunningState::input(Player &player, InputEvent inputEvent)
{
    switch (inputEvent)
    {
        case InputEvent::MoveLeft_Pressed: player.m_velocity.x -= player.speed; break;
        case InputEvent::MoveRight_Pressed:  player.m_velocity.x += player.speed; break;
        case InputEvent::MoveLeft_Released: player.m_velocity.x += player.speed; break;
        case InputEvent::MoveRight_Released: player.m_velocity.x -= player.speed; break;
        default: break;
    }

    if (!player.m_inputManager->isKeyPressed(Input::MoveLeft) &&
        !player.m_inputManager->isKeyPressed(Input::MoveRight))
    {
        player.changeState(PlayerStateName::Idle);
    }
}

void RunningState::update(Player &player, int deltaTime)
{
    player.m_position.x += deltaTime * player.m_velocity.x;

    Animation *animation = player.m_sprite->getAnimator()->getAnimation(PLAYER_ANIMATION_TAG_RUNNING);

    if (player.m_velocity.x > 0)
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

PlayerStateName RunningState::name()
{
    return PlayerStateName::Running;
}

SDL_Texture *RunningState::texture()
{
    return Resources::textures.getTexture(Constants::FILE_SPRITE_PLAYER_RUNNING);
}

Player::Player()
{
    m_inputManager = new InputManager();
    m_currentState = new IdleState();

    m_transform.width = 9;
    m_transform.height = 21;
    m_transform.scale = 1;

    std::vector<Frame> idleAnimationFrames = {
        {
            9,                                                                     // entityWidth
            21,                                                                    // entityHeight
            {0, 0, 0, 0},                                                          // entityHitBox
            {0, 0, 9, 21},                                                         // entityHurtBox
            scaleRect(createClipFromSpriteSheet(32, 32, 11, 7, 9, 21, 0, 0), 15)   // textureClip
        },
        {
            9,
            21,
            {0, 0, 0, 0},
            {0, 0, 9, 21},
            scaleRect(createClipFromSpriteSheet(32, 32, 11, 7, 9, 21, 0, 1), 15),
        },
        {
            9,
            21,
            {0, 0, 0, 0},
            {0, 0, 9, 21},
            scaleRect(createClipFromSpriteSheet(32, 32, 11, 7, 9, 21, 0, 2), 15)
        },
        {
            9,
            21,
            {0, 0, 0, 0},
            {0, 0, 9, 21},
            scaleRect(createClipFromSpriteSheet(32, 32, 11, 7, 9, 21, 1, 0), 15)
        },
        {
            9,
            21,
            {0, 0, 0, 0},
            {0, 0, 9, 21},
            scaleRect(createClipFromSpriteSheet(32, 32, 11, 7, 9, 21, 1, 1), 15)
        },
        {
            9,
            21,
            {0, 0, 0, 0},
            {0, 0, 9, 21},
            scaleRect(createClipFromSpriteSheet(32, 32, 11, 7, 9, 21, 1, 2), 15)
        }
    };

    std::vector<Frame> runningAnimationFrames = {
        {
            9,
            21,
            {0, 0, 0, 0},
            {0, 0, 9, 21},
            scaleRect(createClipFromSpriteSheet(32, 32, 11, 7, 9, 21, 0, 0), 15)
        },
        {
            9,
            21,
            {0, 0, 0, 0},
            {0, 0, 9, 21},
            scaleRect(createClipFromSpriteSheet(32, 32, 11, 7, 9, 21, 0, 1), 15)
        },
        {
            9,
            21,
            {0, 0, 0, 0},
            {0, 0, 9, 21},
            scaleRect(createClipFromSpriteSheet(32, 32, 11, 7, 9, 21, 0, 2), 15)
        },
        {
            9,
            21,
            {0, 0, 0, 0},
            {0, 0, 9, 21},
            scaleRect(createClipFromSpriteSheet(32, 32, 11, 7, 9, 21, 1, 0), 15)
        },
        {
            9,
            21,
            {0, 0, 0, 0},
            {0, 0, 9, 21},
            scaleRect(createClipFromSpriteSheet(32, 32, 11, 7, 9, 21, 1, 1), 15)
        },
        {
            9,
            21,
            {0, 0, 0, 0},
            {0, 0, 9, 21},
            scaleRect(createClipFromSpriteSheet(32, 32, 11, 7, 9, 21, 1, 2), 15)
        },
        {
            9,
            21,
            {0, 0, 0, 0},
            {0, 0, 9, 21},
            scaleRect(createClipFromSpriteSheet(32, 32, 11, 7, 9, 21, 2, 0), 15)
        },
        {
            9,
            21,
            {0, 0, 0, 0},
            {0, 0, 9, 21},
            scaleRect(createClipFromSpriteSheet(32, 32, 11, 7, 9, 21, 2, 1), 15)
        }
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
    delete m_sprite;
}

void Player::setPosition(float x, float y)
{
    m_position.x = x;
    m_position.y = y;
}

void Player::setVelocity(float x, float y)
{
    m_velocity.x = x;
    m_velocity.y = y;
}

void Player::setScale(int scale)
{
    m_transform.scale = scale;
}

void Player::input(InputEvent inputEvent)
{
    m_inputManager->input(inputEvent);
    m_currentState->input(*this, inputEvent);
}

void Player::update(int deltaTime)
{
    m_currentState->update(*this, deltaTime);
    m_sprite->getAnimator()->updateCurrentAnimation(deltaTime);
    boundPosition();
}

void Player::render(SDL_Renderer *renderer, bool drawHitBox, bool drawHurtBox)
{
    SDL_Rect clipRect;
    SDL_Rect renderRect;

    Animation *animation = m_sprite->getAnimator()->getCurrentAnimation();
    SDL_Texture *texture = m_currentState->texture();

    renderRect.x = m_position.x;
    renderRect.y = m_position.y;
    renderRect.w = m_transform.width * m_transform.scale;
    renderRect.h = m_transform.height * m_transform.scale;

    clipRect = animation->getCurrentFrame().textureClip;

    SDL_RenderCopyEx(renderer, texture, &clipRect, &renderRect, 0.0, NULL, animation->getFlip());

    if (drawHitBox)
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue

        SDL_Rect renderHitBox = getRenderSpaceHitBox();
        if (renderHitBox.w * renderHitBox.h > 0)
        {
            SDL_RenderDrawRect(renderer, &renderHitBox);
        }
    }
    if (drawHurtBox)
    {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red

        SDL_Rect renderHurtBox = getRenderSpaceHurtBox();
        if (renderHurtBox.w * renderHurtBox.h > 0)
        {
            SDL_RenderDrawRect(renderer, &renderHurtBox);
        }
    }
}

SDL_Rect Player::getRenderSpaceHitBox()
{
    SDL_Rect entityHitBox = m_sprite->getAnimator()->getCurrentAnimation()->getCurrentFrame().entityHitBox;

    SDL_Rect renderHurtBox;
    renderHurtBox.x = m_position.x + (entityHitBox.x * m_transform.scale);
    renderHurtBox.y = m_position.y + (entityHitBox.y * m_transform.scale);
    renderHurtBox.w = entityHitBox.w * m_transform.scale;
    renderHurtBox.h = entityHitBox.h * m_transform.scale;

    return renderHurtBox;
}

SDL_Rect Player::getRenderSpaceHurtBox()
{
    SDL_Rect entityHurtBox = m_sprite->getAnimator()->getCurrentAnimation()->getCurrentFrame().entityHurtBox;

    SDL_Rect renderHurtBox;
    renderHurtBox.x = m_position.x + (entityHurtBox.x * m_transform.scale);
    renderHurtBox.y = m_position.y + (entityHurtBox.y * m_transform.scale);
    renderHurtBox.w = entityHurtBox.w * m_transform.scale;
    renderHurtBox.h = entityHurtBox.h * m_transform.scale;

    return renderHurtBox;
}

void Player::changeState(PlayerStateName state)
{
    m_currentState->exit(*this);
    delete m_currentState;

    switch (state)
    {
        case PlayerStateName::Idle:
            m_currentState = new IdleState();
            break;
        case PlayerStateName::Running:
            m_currentState = new RunningState();
            break;
        case PlayerStateName::None:
            throw std::logic_error("Can't change Player state to 'None'");
    }

    m_currentState->enter(*this);
}

void Player::boundPosition()
{
    if (m_position.x < 0)
    {
        m_position.x = 0;
    }
    else if (m_position.x > (Constants::WINDOW_WIDTH - (m_transform.width * m_transform.scale)))
    {
        m_position.x = Constants::WINDOW_WIDTH - (m_transform.width * m_transform.scale);
    }
}

PlayerStateName Player::getState()
{
    return m_currentState->name();
}
