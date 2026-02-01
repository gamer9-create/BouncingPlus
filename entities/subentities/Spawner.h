//
// Created by lalit on 10/28/2025.
//

#ifndef BOUNCINGPLUS_SPAWNER_H
#define BOUNCINGPLUS_SPAWNER_H
#include "../Entity.h"

class Spawner : public Entity{
public:
    Vector2 StartPos;
    Vector2 RandPoint;
    int EntitiesSpawned;
    float PosMultiplier;
    float RotMultiplier;
    double SpawnTimer;
    double SpawnerIsActive;
    double SpawnerRageCooldown;

    float EnemyDifficulty;
    double SpawnCooldown;

    double timer;
    float DistF;
    float RandomNumbers[8];
    Spawner(Game &game, float bbox_x, float bbox_y);
    Spawner();
    virtual ~Spawner();
    void Update();
    void Render();
};


#endif //BOUNCINGPLUS_SPAWNER_H