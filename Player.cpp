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


void PlayerStateIdle::enter(Player &player)
{

}

void PlayerStateIdle::input(Player &player, InputEvent inputEvent)
{
    switch (inputEvent)
    {
        case InputEvent::Attack_Pressed:
            player.changeState(PlayerState::Attack);
            break;

        case InputEvent::MoveLeft_Pressed:
            player.m_velocity.x -= player.SPEED;

            if (
                player.m_inputManager.isKeyPressed(Input::MoveLeft) ||
                player.m_inputManager.isKeyPressed(Input::MoveRight)
            )
            {
                player.changeState(PlayerState::Run);
            }

            break;

        case InputEvent::MoveRight_Pressed:
            player.m_velocity.x += player.SPEED;

            if (
                player.m_inputManager.isKeyPressed(Input::MoveLeft) ||
                player.m_inputManager.isKeyPressed(Input::MoveRight)
            )
            {
                player.changeState(PlayerState::Run);
            }

            break;

        default:
            break;
    }
}

void PlayerStateIdle::update(Player &player, int deltaTime)
{
    player.m_animationManager.getAnimation(PlayerState::Idle)->update(deltaTime);
}

void PlayerStateIdle::exit(Player &player)
{
    player.m_animationManager.getAnimation(PlayerState::Idle)->reset();
}

SDL_Texture *PlayerStateIdle::getTexture()
{
    return Resources::textures.getTexture(Constants::FILE_SPRITE_PLAYER_IDLE);
}

std::shared_ptr<Animation> PlayerStateIdle::getAnimation(const Player &player)
{
    return player.m_animationManager.getAnimation(PlayerState::Idle);
}

void PlayerStateRun::enter(Player &player)
{

}

void PlayerStateRun::input(Player &player, InputEvent inputEvent)
{
    switch (inputEvent)
    {
        case InputEvent::Attack_Pressed:
            if (
                !player.m_inputManager.isKeyPressed(Input::MoveLeft) &&
                !player.m_inputManager.isKeyPressed(Input::MoveRight)
            )
            {
                player.changeState(PlayerState::Attack);
            }
            break;

        case InputEvent::MoveLeft_Pressed:
            player.m_velocity.x -= player.SPEED;
            break;

        case InputEvent::MoveRight_Pressed:
            player.m_velocity.x += player.SPEED;
            break;

        case InputEvent::MoveLeft_Released:
            player.m_velocity.x += player.SPEED;

            if (
                !player.m_inputManager.isKeyPressed(Input::MoveLeft) &&
                !player.m_inputManager.isKeyPressed(Input::MoveRight)
            )
            {
                if (player.m_inputManager.isKeyPressed(Input::Attack))
                {
                    player.changeState(PlayerState::Attack);
                }
                else
                {
                    player.changeState(PlayerState::Idle);
                }

                player.updateDirection(Direction::Left);
            }

            break;

        case InputEvent::MoveRight_Released:
            player.m_velocity.x -= player.SPEED;

            if (
                !player.m_inputManager.isKeyPressed(Input::MoveLeft) &&
                !player.m_inputManager.isKeyPressed(Input::MoveRight)
            )
            {
                if (player.m_inputManager.isKeyPressed(Input::Attack))
                {
                    player.changeState(PlayerState::Attack);
                }
                else
                {
                    player.changeState(PlayerState::Idle);
                }

                player.updateDirection(Direction::Right);
            }

            break;

        default:
            break;
    }
}

void PlayerStateRun::update(Player &player, int deltaTime)
{
    player.m_position.x += deltaTime * player.m_velocity.x;

    if (player.m_velocity.x > 0)
    {
        player.updateDirection(Direction::Right);
    }
    else
    {
        player.updateDirection(Direction::Left);
    }

    player.m_animationManager.getAnimation(PlayerState::Run)->update(deltaTime);
}

void PlayerStateRun::exit(Player &player)
{
    player.m_animationManager.getAnimation(PlayerState::Run)->reset();
}

SDL_Texture *PlayerStateRun::getTexture()
{
    return Resources::textures.getTexture(Constants::FILE_SPRITE_PLAYER_RUNNING);
}

std::shared_ptr<Animation> PlayerStateRun::getAnimation(const Player &player)
{
    return player.m_animationManager.getAnimation(PlayerState::Run);
}

void PlayerStateAttack::enter(Player &player)
{

}

void PlayerStateAttack::input(Player &player, InputEvent inputEvent)
{
    switch (inputEvent)
    {
        case InputEvent::Attack_Pressed:
            player.changeState(PlayerState::Attack);
            break;

        case InputEvent::MoveLeft_Pressed:
            player.m_velocity.x -= player.SPEED;

            if (
                player.m_inputManager.isKeyPressed(Input::MoveLeft) ||
                player.m_inputManager.isKeyPressed(Input::MoveRight)
            )
            {
                player.changeState(PlayerState::Run);
            }

            break;
        case InputEvent::MoveRight_Pressed:
            player.m_velocity.x += player.SPEED;

            if (
                player.m_inputManager.isKeyPressed(Input::MoveLeft) ||
                player.m_inputManager.isKeyPressed(Input::MoveRight)
            )
            {
                player.changeState(PlayerState::Run);
            }

            break;

        default:
            break;
    }
}

void PlayerStateAttack::update(Player &player, int deltaTime)
{
    if (player.m_animationManager.getAnimation(PlayerState::Attack)->isDone())
    {
        player.changeState(PlayerState::Idle);
    }
    else
    {
        player.m_animationManager.getAnimation(PlayerState::Attack)->update(deltaTime);
    }
}

void PlayerStateAttack::exit(Player &player)
{
    player.m_animationManager.getAnimation(PlayerState::Attack)->reset();
}

SDL_Texture *PlayerStateAttack::getTexture()
{
    return Resources::textures.getTexture(Constants::FILE_SPRITE_PLAYER_ATTACK);
}

std::shared_ptr<Animation> PlayerStateAttack::getAnimation(const Player &player)
{
    return player.m_animationManager.getAnimation(PlayerState::Attack);
}

void PlayerStateKnockback::enter(Player &player)
{
    player.m_isImmune = true;
}

void PlayerStateKnockback::input(Player &player, InputEvent inputEvent)
{
    // Ignore inputs during knockback
}

void PlayerStateKnockback::update(Player &player, int deltaTime)
{
    if (player.m_animationManager.getAnimation(PlayerState::Knockback)->isDone())
    {
        player.changeState(PlayerState::Idle);
    }
    else
    {
        player.m_animationManager.getAnimation(PlayerState::Knockback)->update(deltaTime);
    }
}

void PlayerStateKnockback::exit(Player &player)
{
    player.m_animationManager.getAnimation(PlayerState::Knockback)->reset();
    player.m_isImmune = false;
}

SDL_Texture *PlayerStateKnockback::getTexture()
{
    return Resources::textures.getTexture(Constants::FILE_SPRITE_PLAYER_KNOCKBACK);
}

std::shared_ptr<Animation> PlayerStateKnockback::getAnimation(const Player &player)
{
    return player.m_animationManager.getAnimation(PlayerState::Knockback);
}

AnimationManager<PlayerState> Player::makeAnimationManager() const
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
        // Frame 03 : When you can deal damage!
        {
            32,
            32,
            {10, 7, 13, 16},
            {22, 9, 8, 9}, // The sword
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

    std::vector<Frame> knockbackAnimationFrames = {
        // Only using knockback's first frame because we go back to the idle after
        // anyway and without the second frame that transition looks smoother
        {
            32,
            32,
            {9, 7, 13, 16},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            scaleRect(createClipFromSpriteSheet(32, 32, 0, 0, 32, 32, 0, 0), 15)
        },
    };

    Animation idleAnimation = Animation(4, idleAnimationFrames);
    Animation runningAnimation = Animation(9, runningAnimationFrames);
    Animation attackAnimation = Animation(7, attackAnimationFrames, false);
    Animation knockbackAnimation = Animation(4, knockbackAnimationFrames, false);

    AnimationManager<PlayerState> animationManager;
    animationManager.addAnimation(PlayerState::Idle, idleAnimation);
    animationManager.addAnimation(PlayerState::Run, runningAnimation);
    animationManager.addAnimation(PlayerState::Attack, attackAnimation);
    animationManager.addAnimation(PlayerState::Knockback, knockbackAnimation);

    return animationManager;
}

Player::Player()
{
    m_stateObject = new PlayerStateIdle();

    m_transform.width = 32;
    m_transform.height = 32;
    m_transform.scale = 1;

    m_animationManager = makeAnimationManager();

    m_stateObject->enter(*this);
}

Player::~Player()
{
    m_stateObject->exit(*this);

    delete m_stateObject;
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
    m_stateObject->input(*this, inputEvent);
}

void Player::update(int deltaTime)
{
    m_stateObject->update(*this, deltaTime);
    boundPosition();
}

void Player::render(SDL_Renderer *renderer, bool drawBoundingBox, bool drawHitBox, bool drawHurtBox) const
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

void Player::renderBox(SDL_Renderer *renderer, SDL_Rect box) const
{
    // SDL Seems to draw a single pixel when the area is 0, but ideally
    // if area is 0 don't draw anything
    if (box.w * box.h > 0)
    {
        SDL_RenderDrawRect(renderer, &box);
    }
}

void Player::renderPlayer(SDL_Renderer *renderer) const
{
    std::shared_ptr<Animation> animation = m_stateObject->getAnimation(*this);

    SDL_Rect clipRect = animation->getCurrentFrame().textureClip;
    SDL_Rect renderRect = transformPlayerToRenderSpace();

    SDL_RenderCopyEx(
        renderer,
        m_stateObject->getTexture(),
        &clipRect,
        &renderRect,
        0.0,
        NULL,
        animation->getFlip()
    );
}

SDL_Rect Player::getBoundingBox() const
{
    return m_stateObject->getAnimation(*this)->getCurrentFrame().entityBoundingBox;
}

SDL_Rect Player::getHitBox() const
{
    return m_stateObject->getAnimation(*this)->getCurrentFrame().entityHitBox;
}

SDL_Rect Player::getHurtBox() const
{
    return m_stateObject->getAnimation(*this)->getCurrentFrame().entityHurtBox;
}

SDL_Rect Player::transformBoxToRenderSpace(SDL_Rect box) const
{
    SDL_Rect renderRect;
    renderRect.x = m_position.x + (box.x * m_transform.scale);
    renderRect.y = m_position.y + (box.y * m_transform.scale);
    renderRect.w = box.w * m_transform.scale;
    renderRect.h = box.h * m_transform.scale;

    return renderRect;
}

SDL_Rect Player::transformPlayerToRenderSpace() const
{
    SDL_Rect renderRect;
    renderRect.x = m_position.x;
    renderRect.y = m_position.y;
    renderRect.w = m_transform.width * m_transform.scale;
    renderRect.h = m_transform.height * m_transform.scale;

    return renderRect;
}

void Player::changeState(PlayerState state)
{
    m_stateObject->exit(*this);
    delete m_stateObject;

    switch (state)
    {
        case PlayerState::Idle:
            m_stateObject = new PlayerStateIdle();
            break;
        case PlayerState::Run:
            m_stateObject = new PlayerStateRun();
            break;
        case PlayerState::Attack:
            m_stateObject = new PlayerStateAttack();
            break;
        case PlayerState::Knockback:
            m_stateObject = new PlayerStateKnockback();
            break;
    }

    m_currentState = state;
    m_stateObject->enter(*this);
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

    m_animationManager.getAnimation(PlayerState::Idle)->setFlip(flip);
    m_animationManager.getAnimation(PlayerState::Run)->setFlip(flip);
    m_animationManager.getAnimation(PlayerState::Attack)->setFlip(flip);
    m_animationManager.getAnimation(PlayerState::Knockback)->setFlip(flip);
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

PlayerState Player::getState() const
{
    return m_currentState;
}

Direction Player::getDirection() const
{
    return m_direction;
}

bool Player::isHitBy(const Player &opponent) const
{
    SDL_Rect playerHurtBox = transformBoxToRenderSpace(getHurtBox());
    SDL_Rect opponentHitBox = opponent.transformBoxToRenderSpace(opponent.getHitBox());

    return SDL_HasIntersection(&playerHurtBox, &opponentHitBox);
}

bool Player::maybeRegisterHit(const Player &opponent)
{
    if (isHitBy(opponent) && !m_isImmune)
    {
        updateDirection((opponent.getDirection() == Direction::Left) ? Direction::Right : Direction::Left);
        changeState(PlayerState::Knockback);

        return true;
    }

    return false;
}
