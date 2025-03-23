#ifndef BATTLE_SCENE_H_
#define BATTLE_SCENE_H_

#include "Components/Components.h"
#include "Player.h"
#include "Wall.h"
#include "Constants.h"

class BattleScene
{
    public:
        BattleScene();
        ~BattleScene();

        void setPlayer(Player* player);
        Player* getPlayer();

        Wall* getLeftWall();
        Wall* getRightWall();

    private:
        Player* m_player;
        Wall* m_leftWall;
        Wall* m_rightWall;
};

#endif