#ifndef VELOCITY_COMPONENT_H_
#define VELOCITY_COMPONENT_H_

class VelocityComponent
{
    public:
        VelocityComponent();
        VelocityComponent(int velX = 0, int velY = 0, int maxVelocity = 0);

        int velX();
        int velY();
        int maxVelocity();

        void setVelX(int velX);
        void setVelY(int velY);

    private:
        int m_velX;
        int m_velY;
        int m_maxVelocity;
};

#endif