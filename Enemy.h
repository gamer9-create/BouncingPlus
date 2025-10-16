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
    WeaponsSystem weaponsSystem;
    bool weaponsSystemInit = false;
    float AnimatedHealth;
    float Armor;
    std::string MyWeapon;
    Enemy(float X, float Y, float Health, float Speed, float Armor, std::string Weapon, Texture2D& EnemyTexture, Game &game);
    Enemy();
    bool Raycast(float target_x, float target_y);
    virtual ~Enemy();
    void Update();
};


#endif //BOUNCINGPLUS_ENEMY_H