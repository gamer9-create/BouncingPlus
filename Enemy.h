//
// Created by lalit on 8/28/2025.
//

#ifndef BOUNCINGPLUS_ENEMY_H
#define BOUNCINGPLUS_ENEMY_H
#include "Entity.h"
#include "Weapons.h"


class Enemy : public Entity {
    float AnimatedHealth;
    std::string MyWeapon;

    WeaponsSystem weaponsSystem;
    bool weaponsSystemInit = false;
public:
    float AngeredRangeBypassTimerMax;
    float AngeredRangeBypassTimer;
    float Armor;
    Enemy(float X, float Y, float Health, float Speed, float Armor, std::string Weapon, Texture2D& EnemyTexture, Game &game);
    Enemy();
    virtual ~Enemy();
    void Update();
    void Wander();
};


#endif //BOUNCINGPLUS_ENEMY_H