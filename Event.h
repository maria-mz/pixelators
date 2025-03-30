#ifndef EVENT_H_
#define EVENT_H_

#include <SDL2/SDL.h>

enum Action {
    ACTION_MOVE_LEFT,
    ACTION_MOVE_RIGHT,
    ACTION_COUNT
};

enum Event {
    EVENT_NONE,
    EVENT_MOVE_LEFT_PRESSED,
    EVENT_MOVE_RIGHT_PRESSED,
    EVENT_MOVE_LEFT_RELEASED,
    EVENT_MOVE_RIGHT_RELEASED,
    EVENT_COUNT
};

class SDLEventTranslator
{
    public:
        static Event translate(SDL_Event &SDLEvent);
};

#endif