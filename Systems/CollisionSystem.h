#ifndef COLLISION_SYSTEM_H_
#define COLLISION_SYSTEM_H_

#include "ISystem.h"
#include "../Components/Components.h"

class CollisionSystem : public ISystem
{
    public:
        bool start() override;
        void input(SDL_Event &e) override;
        void update(int time) override;
        void end() override;
};

#endif