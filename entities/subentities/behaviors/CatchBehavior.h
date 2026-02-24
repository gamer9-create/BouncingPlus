//
// Created by lalit on 2/2/2026.
//

#ifndef BOUNCINGPLUS_CATCHBEHAVIOR_H
#define BOUNCINGPLUS_CATCHBEHAVIOR_H
#include <raylib.h>

#include "EnemyBehavior.h"


class CatchBehavior : public EnemyBehavior
{
    void ResetDirection();
public:
    Vector2 currentDir = {0, 0};
    float currentSpeed = 0;
    Vector2 lastPosition = {0, 0};
    double lastTime = 0;
    double attackCooldown = 0;
    CatchBehavior();
    CatchBehavior(Enemy& Owner, Game& game);
    ~CatchBehavior();
    void Update();
};


#endif //BOUNCINGPLUS_CATCHBEHAVIOR_H