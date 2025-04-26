#include "Player.h"

InputManager::InputManager()
{
    m_keyState = {
        {Input::MoveLeft, false},
        {Input::MoveRight, false},
        {Input::Attack, false}
    };
}

void InputManager::input(InputEvent inputEvent)
{
    switch (inputEvent)
    {
        case InputEvent::MoveLeft_Pressed: m_keyState[Input::MoveLeft] = true; break;
        case InputEvent::MoveRight_Pressed: m_keyState[Input::MoveRight] = true; break;
        case InputEvent::Attack_Pressed: m_keyState[Input::Attack] = true; break;
        case InputEvent::MoveLeft_Released: m_keyState[Input::MoveLeft] = false; break;
        case InputEvent::MoveRight_Released: m_keyState[Input::MoveRight] = false; break;
        case InputEvent::Attack_Released: m_keyState[Input::Attack] = false; break;
        default: break;
    }
}

bool InputManager::isKeyPressed(Input action)
{
    return m_keyState[action];
}


void IdleState::enter(Player &player)
{
    player.m_sprite.getAnimator()->setCurrentAnimation(PLAYER_ANIMATION_TAG_IDLE);
}

void IdleState::input(Player &player, InputEvent inputEvent)
{
    switch (inputEvent)
    {
        case InputEvent::Attack_Pressed:
            player.changeState(PlayerStateName::Attack);
            break;

        case InputEvent::MoveLeft_Pressed:
            player.m_velocity.x -= player.speed;

            if (
                player.m_inputManager.isKeyPressed(Input::MoveLeft) ||
                player.m_inputManager.isKeyPressed(Input::MoveRight)
            )
            {
                player.changeState(PlayerStateName::Running);
            }

            break;

        case InputEvent::MoveRight_Pressed:
            player.m_velocity.x += player.speed;

            if (
                player.m_inputManager.isKeyPressed(Input::MoveLeft) ||
                player.m_inputManager.isKeyPressed(Input::MoveRight)
            )
            {
                player.changeState(PlayerStateName::Running);
            }

            break;

        default:
            break;
    }
}

void IdleState::update(Player &player, int deltaTime)
{
    player.m_sprite.getAnimator()->updateCurrentAnimation(deltaTime);
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
    player.m_sprite.getAnimator()->setCurrentAnimation(PLAYER_ANIMATION_TAG_RUNNING);
}

void RunningState::input(Player &player, InputEvent inputEvent)
{
    switch (inputEvent)
    {
        case InputEvent::Attack_Pressed:
            if (
                !player.m_inputManager.isKeyPressed(Input::MoveLeft) &&
                !player.m_inputManager.isKeyPressed(Input::MoveRight)
            )
            {
                player.changeState(PlayerStateName::Attack);
            }
            break;

        case InputEvent::MoveLeft_Pressed:
            player.m_velocity.x -= player.speed;
            break;

        case InputEvent::MoveRight_Pressed:
            player.m_velocity.x += player.speed;
            break;

        case InputEvent::MoveLeft_Released:
            player.m_velocity.x += player.speed;

            if (
                !player.m_inputManager.isKeyPressed(Input::MoveLeft) &&
                !player.m_inputManager.isKeyPressed(Input::MoveRight)
            )
            {
                if (player.m_inputManager.isKeyPressed(Input::Attack))
                {
                    player.changeState(PlayerStateName::Attack);
                }
                else
                {
                    player.changeState(PlayerStateName::Idle);
                }

                player.updateDirection(Direction::Left);
            }

            break;

        case InputEvent::MoveRight_Released:
            player.m_velocity.x -= player.speed;

            if (
                !player.m_inputManager.isKeyPressed(Input::MoveLeft) &&
                !player.m_inputManager.isKeyPressed(Input::MoveRight)
            )
            {
                if (player.m_inputManager.isKeyPressed(Input::Attack))
                {
                    player.changeState(PlayerStateName::Attack);
                }
                else
                {
                    player.changeState(PlayerStateName::Idle);
                }

                player.updateDirection(Direction::Right);
            }

            break;

        default:
            break;
    }
}

void RunningState::update(Player &player, int deltaTime)
{
    player.m_position.x += deltaTime * player.m_velocity.x;

    Animation *animation = player.m_sprite.getAnimator()->getAnimation(PLAYER_ANIMATION_TAG_RUNNING);

    if (player.m_velocity.x > 0)
    {
        player.updateDirection(Direction::Right);
    }
    else
    {
        player.updateDirection(Direction::Left);
    }

    player.m_sprite.getAnimator()->updateCurrentAnimation(deltaTime);
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

void AttackState::enter(Player &player)
{
    player.m_sprite.getAnimator()->setCurrentAnimation(PLAYER_ANIMATION_TAG_ATTACK);
}

void AttackState::input(Player &player, InputEvent inputEvent)
{
    switch (inputEvent)
    {
        case InputEvent::Attack_Pressed:
            player.changeState(PlayerStateName::Attack);
            break;

        case InputEvent::MoveLeft_Pressed:
            player.m_velocity.x -= player.speed;

            if (
                player.m_inputManager.isKeyPressed(Input::MoveLeft) ||
                player.m_inputManager.isKeyPressed(Input::MoveRight)
            )
            {
                player.changeState(PlayerStateName::Running);
            }

            break;
        case InputEvent::MoveRight_Pressed:
            player.m_velocity.x += player.speed;

            if (
                player.m_inputManager.isKeyPressed(Input::MoveLeft) ||
                player.m_inputManager.isKeyPressed(Input::MoveRight)
            )
            {
                player.changeState(PlayerStateName::Running);
            }

            break;

        default:
            break;
    }
}

void AttackState::update(Player &player, int deltaTime)
{
    if (player.m_sprite.getAnimator()->getCurrentAnimation()->isDone())
    {
        player.changeState(PlayerStateName::Idle);
    }
    else
    {
        player.m_sprite.getAnimator()->updateCurrentAnimation(deltaTime);
    }
}

void AttackState::exit(Player &player)
{

}

PlayerStateName AttackState::name()
{
    return PlayerStateName::Attack;
}

SDL_Texture *AttackState::texture()
{
    return Resources::textures.getTexture(Constants::FILE_SPRITE_PLAYER_ATTACK);
}

Animator *Player::makeAnimator()
{
    std::vector<Frame> idleAnimationFrames = {
        {
            32,                                                                    // entityWidth
            32,                                                                    // entityHeight
            {10, 7, 13, 16},                                                       // entityBoundingBox
            {0, 0, 0, 0},                                                          // entityHitBox
            {8, 5, 17, 20},                                                        // entityHurtBox
            scaleRect(createClipFromSpriteSheet(32, 32, 0, 0, 32, 32, 0, 0), 15)   // textureClip
        },
        {
            32,
            32,
            {10, 7, 13, 16},
            {0, 0, 0, 0},
            {8, 5, 17, 20},
            scaleRect(createClipFromSpriteSheet(32, 32, 0, 0, 32, 32, 0, 1), 15),
        },
    };

    std::vector<Frame> runningAnimationFrames = {
        {
            32,
            32,
            {10, 7, 13, 16},
            {0, 0, 0, 0},
            {8, 5, 17, 20},
            scaleRect(createClipFromSpriteSheet(32, 32, 0, 0, 32, 32, 0, 0), 15)
        },
        {
            32,
            32,
            {10, 7, 13, 16},
            {0, 0, 0, 0},
            {8, 5, 17, 20},
            scaleRect(createClipFromSpriteSheet(32, 32, 0, 0, 32, 32, 0, 1), 15)
        },
        {
            32,
            32,
            {10, 7, 13, 16},
            {0, 0, 0, 0},
            {8, 5, 17, 20},
            scaleRect(createClipFromSpriteSheet(32, 32, 0, 0, 32, 32, 0, 2), 15)
        },
        {
            32,
            32,
            {10, 7, 13, 16},
            {0, 0, 0, 0},
            {8, 5, 17, 20},
            scaleRect(createClipFromSpriteSheet(32, 32, 0, 0, 32, 32, 1, 0), 15)
        },
        {
            32,
            32,
            {10, 7, 13, 16},
            {0, 0, 0, 0},
            {8, 5, 17, 20},
            scaleRect(createClipFromSpriteSheet(32, 32, 0, 0, 32, 32, 1, 1), 15)
        },
        {
            32,
            32,
            {10, 7, 13, 16},
            {0, 0, 0, 0},
            {8, 5, 17, 20},
            scaleRect(createClipFromSpriteSheet(32, 32, 0, 0, 32, 32, 1, 2), 15)
        },
        {
            32,
            32,
            {10, 7, 13, 16},
            {0, 0, 0, 0},
            {8, 5, 17, 20},
            scaleRect(createClipFromSpriteSheet(32, 32, 0, 0, 32, 32, 2, 0), 15)
        },
        {
            32,
            32,
            {10, 7, 13, 16},
            {0, 0, 0, 0},
            {8, 5, 17, 20},
            scaleRect(createClipFromSpriteSheet(32, 32, 0, 0, 32, 32, 2, 1), 15)
        },
    };

    std::vector<Frame> attackAnimationFrames = {
        {
            32,
            32,
            {10, 7, 13, 16},
            {0, 0, 0, 0},
            {8, 5, 17, 20},
            scaleRect(createClipFromSpriteSheet(32, 32, 0, 0, 32, 32, 0, 0), 15)
        },
        {
            32,
            32,
            {10, 7, 13, 16},
            {0, 0, 0, 0},
            {8, 5, 17, 20},
            scaleRect(createClipFromSpriteSheet(32, 32, 0, 0, 32, 32, 0, 1), 15)
        },
        {
            32,
            32,
            {10, 7, 13, 16},
            {0, 0, 0, 0},
            {8, 5, 17, 20},
            scaleRect(createClipFromSpriteSheet(32, 32, 0, 0, 32, 32, 1, 0), 15)
        },
        {
            32,
            32,
            {10, 7, 13, 16},
            {0, 0, 0, 0},
            {8, 5, 17, 20},
            scaleRect(createClipFromSpriteSheet(32, 32, 0, 0, 32, 32, 1, 1), 15)
        },
    };

    Animation *idleAnimation = new Animation(4);
    Animation *runningAnimation = new Animation(8);
    Animation *attackAnimation = new Animation(7, false);

    idleAnimation->setFrames(idleAnimationFrames);
    runningAnimation->setFrames(runningAnimationFrames);
    attackAnimation->setFrames(attackAnimationFrames);

    Animator *animator = new Animator();
    animator->addAnimation(PLAYER_ANIMATION_TAG_IDLE, idleAnimation);
    animator->addAnimation(PLAYER_ANIMATION_TAG_RUNNING, runningAnimation);
    animator->addAnimation(PLAYER_ANIMATION_TAG_ATTACK, attackAnimation);

    return animator;
}

Player::Player()
{
    m_currentState = new IdleState();

    m_transform.width = 32;
    m_transform.height = 32;
    m_transform.scale = 1;

    Animator *animator = makeAnimator();

    m_sprite.setAnimator(animator);

    m_currentState->enter(*this);
}

Player::~Player()
{
    m_currentState->exit(*this);

    delete m_currentState;
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
    m_inputManager.input(inputEvent);
    m_currentState->input(*this, inputEvent);
}

void Player::update(int deltaTime)
{
    m_currentState->update(*this, deltaTime);
    boundPosition();
}

void Player::render(SDL_Renderer *renderer, bool drawBoundingBox, bool drawHitBox, bool drawHurtBox)
{
    renderPlayer(renderer);

    if (drawBoundingBox)
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black
        renderBox(renderer, transformBoxToRenderSpace(getBoundingBox()));
    }
    if (drawHitBox)
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue
        renderBox(renderer, transformBoxToRenderSpace(getHitBox()));
    }
    if (drawHurtBox)
    {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red
        renderBox(renderer, transformBoxToRenderSpace(getHurtBox()));
    }
}

void Player::renderBox(SDL_Renderer *renderer, SDL_Rect box)
{
    // SDL Seems to draw a single pixel when the area is 0, but ideally
    // if area is 0 don't draw anything
    if (box.w * box.h > 0)
    {
        SDL_RenderDrawRect(renderer, &box);
    }
}

void Player::renderPlayer(SDL_Renderer *renderer)
{
    SDL_Rect clipRect;
    SDL_Rect renderRect;

    Animation *animation = m_sprite.getAnimator()->getCurrentAnimation();
    SDL_Texture *texture = m_currentState->texture();

    renderRect.x = m_position.x;
    renderRect.y = m_position.y;
    renderRect.w = m_transform.width * m_transform.scale;
    renderRect.h = m_transform.height * m_transform.scale;

    clipRect = animation->getCurrentFrame().textureClip;

    SDL_RenderCopyEx(renderer, texture, &clipRect, &renderRect, 0.0, NULL, animation->getFlip());
}

SDL_Rect Player::getBoundingBox()
{
    return m_sprite.getAnimator()->getCurrentAnimation()->getCurrentFrame().entityBoundingBox;
}

SDL_Rect Player::getHitBox()
{
    return m_sprite.getAnimator()->getCurrentAnimation()->getCurrentFrame().entityHitBox;
}

SDL_Rect Player::getHurtBox()
{
    return m_sprite.getAnimator()->getCurrentAnimation()->getCurrentFrame().entityHurtBox;
}

SDL_Rect Player::transformBoxToRenderSpace(SDL_Rect box)
{
    SDL_Rect renderBox;
    renderBox.x = m_position.x + (box.x * m_transform.scale);
    renderBox.y = m_position.y + (box.y * m_transform.scale);
    renderBox.w = box.w * m_transform.scale;
    renderBox.h = box.h * m_transform.scale;

    return renderBox;
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
        case PlayerStateName::Attack:
            m_currentState = new AttackState();
            break;
        case PlayerStateName::None:
            throw std::logic_error("Can't change Player state to 'None'");
    }

    m_currentState->enter(*this);
}

void Player::updateDirection(Direction direction)
{
    m_direction = direction;

    SDL_RendererFlip flip;

    switch (direction)
    {
        case Direction::Right: flip = SDL_FLIP_NONE; break;
        case Direction::Left: flip = SDL_FLIP_HORIZONTAL; break;
    }

    m_sprite.getAnimator()->getAnimation(PLAYER_ANIMATION_TAG_IDLE)->setFlip(flip);
    m_sprite.getAnimator()->getAnimation(PLAYER_ANIMATION_TAG_RUNNING)->setFlip(flip);
    m_sprite.getAnimator()->getAnimation(PLAYER_ANIMATION_TAG_ATTACK)->setFlip(flip);
}

void Player::boundPosition()
{
    SDL_Rect boundingBox = getBoundingBox();

    if (m_position.x + (boundingBox.x * m_transform.scale) < 0)
    {
        m_position.x = -(boundingBox.x * m_transform.scale);
    }
    else if (m_position.x + ((boundingBox.x + boundingBox.w) * m_transform.scale) > Constants::WINDOW_WIDTH)
    {
        m_position.x = Constants::WINDOW_WIDTH - ((boundingBox.x + boundingBox.w) * m_transform.scale);
    }
}

PlayerStateName Player::getState()
{
    return m_currentState->name();
}
