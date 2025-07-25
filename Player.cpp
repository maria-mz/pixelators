#include "Player.h"

InputManager::InputManager()
{
    m_keyState = {
        {Input::MoveLeft, false},
        {Input::MoveRight, false},
        {Input::Attack, false},
        {Input::Block, false}
    };
}

void InputManager::input(InputEvent inputEvent)
{
    switch (inputEvent)
    {
        case InputEvent::MoveLeft_Pressed: m_keyState[Input::MoveLeft] = true; break;
        case InputEvent::MoveRight_Pressed: m_keyState[Input::MoveRight] = true; break;
        case InputEvent::Attack_Pressed: m_keyState[Input::Attack] = true; break;
        case InputEvent::Block_Pressed: m_keyState[Input::Block] = true; break;
        case InputEvent::MoveLeft_Released: m_keyState[Input::MoveLeft] = false; break;
        case InputEvent::MoveRight_Released: m_keyState[Input::MoveRight] = false; break;
        case InputEvent::Attack_Released: m_keyState[Input::Attack] = false; break;
        case InputEvent::Block_Released: m_keyState[Input::Block] = false; break;
        default: break;
    }
}

bool InputManager::isKeyPressed(Input action)
{
    return m_keyState[action];
}

void setPlayerTexturesRed(Player &player)
{
    player.setTexture(PlayerState::Idle,
                      Resources::textures.getTexture(Constants::FILE_SPRITE_PLAYER_IDLE_RED));
    player.setTexture(PlayerState::Run,
                      Resources::textures.getTexture(Constants::FILE_SPRITE_PLAYER_RUN_RED));
    player.setTexture(PlayerState::Attack,
                      Resources::textures.getTexture(Constants::FILE_SPRITE_PLAYER_ATTACK_RED));
    player.setTexture(PlayerState::Block,
                      Resources::textures.getTexture(Constants::FILE_SPRITE_PLAYER_BLOCK_RED));
    player.setTexture(PlayerState::Knockback,
                      Resources::textures.getTexture(Constants::FILE_SPRITE_PLAYER_KNOCKBACK_RED));
}

void setPlayerTexturesBlue(Player &player)
{
    player.setTexture(PlayerState::Idle,
                      Resources::textures.getTexture(Constants::FILE_SPRITE_PLAYER_IDLE_BLUE));
    player.setTexture(PlayerState::Run,
                      Resources::textures.getTexture(Constants::FILE_SPRITE_PLAYER_RUN_BLUE));
    player.setTexture(PlayerState::Attack,
                      Resources::textures.getTexture(Constants::FILE_SPRITE_PLAYER_ATTACK_BLUE));
    player.setTexture(PlayerState::Block,
                      Resources::textures.getTexture(Constants::FILE_SPRITE_PLAYER_BLOCK_BLUE));
    player.setTexture(PlayerState::Knockback,
                      Resources::textures.getTexture(Constants::FILE_SPRITE_PLAYER_KNOCKBACK_BLUE));
}

void PlayerStateIdle::enter(Player &player)
{

}

void PlayerStateIdle::input(Player &player, InputEvent inputEvent)
{
    switch (inputEvent)
    {
        case InputEvent::Block_Pressed:
            player.changeState(PlayerState::Block);
            break;

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

SDL_Texture *PlayerStateIdle::getTexture(const Player &player)
{
    return player.getTexture(PlayerState::Idle);
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
        case InputEvent::Block_Pressed:
            if (
                !player.m_inputManager.isKeyPressed(Input::MoveLeft) &&
                !player.m_inputManager.isKeyPressed(Input::MoveRight)
            )
            {
                player.changeState(PlayerState::Block);
            }
            break;

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
                else if (player.m_inputManager.isKeyPressed(Input::Block))
                {
                    player.changeState(PlayerState::Block);
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
                else if (player.m_inputManager.isKeyPressed(Input::Block))
                {
                    player.changeState(PlayerState::Block);
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

SDL_Texture *PlayerStateRun::getTexture(const Player &player)
{
    return player.getTexture(PlayerState::Run);
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
        case InputEvent::Block_Pressed:
            player.changeState(PlayerState::Block);
            break;

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

SDL_Texture *PlayerStateAttack::getTexture(const Player &player)
{
    return player.getTexture(PlayerState::Attack);
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

SDL_Texture *PlayerStateKnockback::getTexture(const Player &player)
{
    return player.getTexture(PlayerState::Knockback);
}

std::shared_ptr<Animation> PlayerStateKnockback::getAnimation(const Player &player)
{
    return player.m_animationManager.getAnimation(PlayerState::Knockback);
}

void PlayerStateBlock::enter(Player &player)
{

}

void PlayerStateBlock::input(Player &player, InputEvent inputEvent)
{
    switch (inputEvent)
    {
        case InputEvent::Block_Released:
            player.changeState(PlayerState::Idle);
            break;

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

void PlayerStateBlock::update(Player &player, int deltaTime)
{
    player.m_animationManager.getAnimation(PlayerState::Block)->update(deltaTime);
}

void PlayerStateBlock::exit(Player &player)
{
    player.m_animationManager.getAnimation(PlayerState::Block)->reset();
}

SDL_Texture *PlayerStateBlock::getTexture(const Player &player)
{
    return player.getTexture(PlayerState::Block);
}

std::shared_ptr<Animation> PlayerStateBlock::getAnimation(const Player &player)
{
    return player.m_animationManager.getAnimation(PlayerState::Block);
}


AnimationManager<PlayerState> Player::makeAnimationManager() const
{
    std::vector<Frame> idleAnimationFrames = {
        {
            32, // entityWidth
            32, // entityHeight
            {10, 7, 13, 16}, // entityBoundingBox
            {0, 0, 0, 0}, // entityHitBox
            {8, 5, 17, 20}, // entityHurtBox
            scaleRect(createClipFromSpriteSheet(32, 32, 0, 0, 32, 32, 0, 1), 15), // textureClip
        },
        {
            32,
            32,
            {10, 7, 13, 16},
            {0, 0, 0, 0},
            {8, 5, 17, 20},
            scaleRect(createClipFromSpriteSheet(32, 32, 0, 0, 32, 32, 0, 0), 15)
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

    std::vector<Frame> blockAnimationFrames = {
        {
            32,
            32,
            {10, 7, 13, 16},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            scaleRect(createClipFromSpriteSheet(32, 32, 0, 0, 32, 32, 0, 0), 15)
        },
        {
            32,
            32,
            {10, 7, 13, 16},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            scaleRect(createClipFromSpriteSheet(32, 32, 0, 0, 32, 32, 0, 1), 15)
        },
    };

    Animation idleAnimation = Animation(3, idleAnimationFrames);
    Animation runningAnimation = Animation(9, runningAnimationFrames);
    Animation attackAnimation = Animation(8, attackAnimationFrames, false);
    Animation knockbackAnimation = Animation(5, knockbackAnimationFrames, false);
    Animation blockAnimation = Animation(7, blockAnimationFrames, false);

    AnimationManager<PlayerState> animationManager;
    animationManager.addAnimation(PlayerState::Idle, idleAnimation);
    animationManager.addAnimation(PlayerState::Run, runningAnimation);
    animationManager.addAnimation(PlayerState::Attack, attackAnimation);
    animationManager.addAnimation(PlayerState::Knockback, knockbackAnimation);
    animationManager.addAnimation(PlayerState::Block, blockAnimation);

    return animationManager;
}

Player::Player()
    : m_stateObject(new PlayerStateIdle()),
      m_transform{32, 32, 1},
      m_health(Constants::PLAYER_MAX_HEALTH),
      m_animationManager(makeAnimationManager())
{
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

void Player::setTexture(const PlayerState state, SDL_Texture *texture)
{
    m_textures[state] = texture;
}

SDL_Texture *Player::getTexture(PlayerState state) const
{
    auto it = m_textures.find(state);
    if (it != m_textures.end())
    {
        return it->second;
    }
    return nullptr;
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
        m_stateObject->getTexture(*this),
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
        case PlayerState::Block:
            m_stateObject = new PlayerStateBlock();
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
    m_animationManager.getAnimation(PlayerState::Block)->setFlip(flip);
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

    return SDL_HasIntersection(&playerHurtBox, &opponentHitBox) && !m_isImmune;
}

void Player::registerHitTaken(const Player &opponent)
{
    updateDirection((opponent.getDirection() == Direction::Left) ? Direction::Right : Direction::Left);
    changeState(PlayerState::Knockback);

    m_health -= HIT_DMG;
    if (m_health < 0)
    {
        m_health = 0;
    }
}

int Player::getHealth()
{
    return m_health;
}
