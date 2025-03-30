#include "Event.h"

Event SDLEventTranslator::translate(SDL_Event &SDLEvent)
{
    Event event = EVENT_NONE;

    if (SDLEvent.type == SDL_KEYDOWN && SDLEvent.key.repeat == 0)
    {
        switch (SDLEvent.key.keysym.sym)
        {
            case SDLK_d: event = EVENT_MOVE_RIGHT_PRESSED; break;
            case SDLK_a: event = EVENT_MOVE_LEFT_PRESSED; break;
        }
    }

    else if (SDLEvent.type == SDL_KEYUP && SDLEvent.key.repeat == 0)
    {
        switch (SDLEvent.key.keysym.sym)
        {
            case SDLK_d: event = EVENT_MOVE_RIGHT_RELEASED; break;
            case SDLK_a: event = EVENT_MOVE_LEFT_RELEASED; break;
        }
    }

    return event;
}