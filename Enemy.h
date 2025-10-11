//
// Created by lalit on 8/28/2025.
//

#ifndef BOUNCINGPLUS_ENEMY_H
#define BOUNCINGPLUS_ENEMY_H
#include "Entity.h"
#include "Weapons.h"


class Enemy : public Entity {
public:
    float AngeredRangeBypassTimerMax;
    float AngeredRangeBypassTimer;
    int AILevel;
    WeaponsSystem weaponsSystem;
    bool weaponsSystemInit = false;
    float AnimatedHealth;
    Enemy(float X, float Y, int AILevel, float Health, float Speed, Texture2D& EnemyTexture, Game &game);
    Enemy();
    virtual ~Enemy();
    void Update();
};


#endif //BOUNCINGPLUS_ENEMY_H