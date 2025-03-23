#ifndef WALL_H_
#define WALL_H_

#include "Components/Components.h"
#include "SDL2/SDL.h"

class Wall
{
    public:
        Wall();
        ~Wall();

        void setTransformComponent(TransformComponent* transform);
        TransformComponent* transformComponent();

    private:
        TransformComponent* m_transform;
};

#endif