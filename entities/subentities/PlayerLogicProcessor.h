//
// Created by lalit on 3/14/2026.
//

#ifndef BOUNCINGPLUS_PLAYERUTILS_H
#define BOUNCINGPLUS_PLAYERUTILS_H
#include <memory>
#include <raylib.h>
#include <vector>
#include "Enemy.h"

class Player;

class PlayerLogicProcessor
{
public:
    std::weak_ptr<Player> Owner;
    std::vector<Vector3> DamageNotifs;
    float DashCooldown = 0;
    double DashTimeStart = 0;
    std::vector<std::weak_ptr<Enemy>> DashedEnemies;
    float PlayerDashLineThickness = 10;
    double ChaseMusicLock = -10;
    std::string CurrentLayer;
    std::string PrevLayer;
    PlayerLogicProcessor();
    PlayerLogicProcessor(std::weak_ptr<Player> Owner);
    ~PlayerLogicProcessor();
    void Update();
    void DamageNotification(Vector2 From);
    void AttackDashedEnemy(std::shared_ptr<Enemy> entity, bool already_attacked);
    void PhysicsUpdate();
    void DashAttacking();
    void DashLogic();
    void ProcessStress();
    void DisplayDamageNotifs();
    void Destroy();
};


#endif //BOUNCINGPLUS_PLAYERUTILS_H