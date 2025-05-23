#ifndef CONSTANTS_H_
#define CONSTANTS_H_

namespace Constants
{
    // Window
    constexpr const char* WINDOW_TITLE = "Pixelators";
    constexpr int WINDOW_WIDTH = 720;
    constexpr int WINDOW_HEIGHT = 480;

    // Assets
    constexpr const char* FILE_SPRITE_PLAYER_IDLE = "assets/player-idle-spritesheet.png";
    constexpr const char* FILE_SPRITE_PLAYER_RUNNING = "assets/player-run-spritesheet.png";
    constexpr const char* FILE_SPRITE_PLAYER_ATTACK = "assets/player-attack-spritesheet.png";
    constexpr const char* FILE_SPRITE_PLAYER_KNOCKBACK = "assets/player-knockback-spritesheet.png";
    constexpr const char* FILE_SPRITE_PLAYER_BLOCK = "assets/player-block-spritesheet.png";
    constexpr const char* FILE_SPRITE_HEALTH_BAR_OK = "assets/health-bar-ok-sprite.png";
    constexpr const char* FILE_SPRITE_HEALTH_BAR_LOW = "assets/health-bar-low-sprite.png";
    constexpr const char* FILE_FONT_MAIN = "assets/FFFFORWA.TTF";
}

#endif