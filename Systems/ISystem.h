#ifndef ISYSTEM_H_
#define ISYSTEM_H_

#include "SDL2/SDL.h"
#include "../Player.h"


class ISystem
{
    public:
        virtual ~ISystem() = 0;
        virtual bool start() = 0;
        virtual void input(SDL_Event &e) = 0;
        virtual void update(int time) = 0;
        virtual void end() = 0;
        void setPlayer(Player* player) { m_player = player; };

    protected:
        Player* m_player;
};

#endif