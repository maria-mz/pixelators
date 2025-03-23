#ifndef INPUT_SYSTEM_H_
#define INPUT_SYSTEM_H_

#include "ISystem.h"
#include "../Components/Components.h"

class InputSystem : public ISystem
{
    public:
        bool start() override;
        void input(SDL_Event &e) override;
        void update(int time) override;
        void end() override;
};

#endif