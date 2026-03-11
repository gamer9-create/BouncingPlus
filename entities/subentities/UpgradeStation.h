//
// Created by lalit on 11/10/2025.
//

#ifndef BOUNCINGPLUS_UPGRADESTATION_H
#define BOUNCINGPLUS_UPGRADESTATION_H

#include "../Entity.h"

class Game;

class UpgradeStation : public Entity{
public:
    UpgradeStation(Game &game, float bbox_x, float bbox_y);
    UpgradeStation();
    virtual ~UpgradeStation();
    void Update();
    void PhysicsUpdate(float dt, double time);
    void Render();
};


#endif //BOUNCINGPLUS_UPGRADESTATION_H