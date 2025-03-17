#ifndef MOVE_SYSTEM_H_
#define MOVE_SYSTEM_H_

#include "ECS.h"

class MoveSystem : public ISystem
{
    public:
        bool start() override;
        void update(int time) override;
        void end() override;
};

#endif