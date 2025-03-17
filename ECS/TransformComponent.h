#ifndef TRANSFORM_COMPONENT_H_
#define TRANSFORM_COMPONENT_H_

class TransformComponent
{
    public:
        TransformComponent();
        TransformComponent(int posX = 0, int posY = 0, int width = 0, int height = 0);

        int getPosX();
        int getPosY();
        int getWidth();
        int getHeight();

        void setPosX(int posX);
        void setPosY(int posY);
        void setWidth(int width);
        void setHeight(int height);

    private:
        int m_posX, m_posY;
        int m_width, m_height;
};

#endif