#ifndef MOVE_SYSTEM_H_
#define MOVE_SYSTEM_H_

#include "ISystem.h"
#include "../Components/Components.h"

class MoveSystem : public ISystem
{
    public:
        bool start() override;
        void input(SDL_Event &e) override;
        void update(int time) override;
        void end() override;
};

#endif