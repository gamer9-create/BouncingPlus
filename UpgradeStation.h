//
// Created by lalit on 11/10/2025.
//

#ifndef BOUNCINGPLUS_UPGRADESTATION_H
#define BOUNCINGPLUS_UPGRADESTATION_H

#include "Entity.h"

class Game;

class UpgradeStation : public Entity{
public:
    float GoalAngle;
    float SecondArmRot;
    bool isAnimating;
    double CooldownTimer;
    double Ani1Timer;
    Vector2 s;
    UpgradeStation(Game &game, float bbox_x, float bbox_y);
    UpgradeStation();
    virtual ~UpgradeStation();
    void AnimateTowards(float Angle);
    void Update();
    void PhysicsUpdate(float dt);
    void Render();
};


#endif //BOUNCINGPLUS_UPGRADESTATION_H