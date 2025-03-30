#ifndef TRANSFORM_H_
#define TRANSFORM_H_

template<typename T>
class Transform
{
    public:
        Transform() : width(T{}), height(T{}) {}
        Transform(T width, T height) : width(width), height(width) {}

        T width;
        T height;
};

#endif