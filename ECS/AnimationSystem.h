#ifndef ANIMATION_SYSTEM_H_
#define ANIMATION_SYSTEM_H_

#include "ECS.h"
#include "Components.h"

class AnimationSystem : public ISystem
{
    public:
        bool start() override;
        void update(int time) override;
        void end() override;
};

#endif