//
// Created by lalit on 9/13/2025.
//

#ifndef BOUNCINGPLUS_WEAPON_H
#define BOUNCINGPLUS_WEAPON_H

#include <memory>
#include <vector>
#include <string>
#include "raylib.h"

using namespace std;

struct Weapon {
    bool isMelee = false;
    float Speed = 0;
    float Damage = 0;
    float Cooldown = 0;
    int Ammo = -1;
};

class WeaponsSystem {
    public:
        Game* game;
        std::weak_ptr<Entity> Owner;
        std::string Weapons[3];
        Weapon* CurrentWeapon;
        int CurrentWeaponIndex;
        float AttackCooldown;
        int AttackAmmo;

        WeaponsSystem(std::shared_ptr<Entity> Owner, Game &game);
        WeaponsSystem();
        virtual ~WeaponsSystem();
        virtual void Equip(int Index);
        virtual void Unequip();
        virtual void Update();
        virtual void Attack(Vector2 Target);
};

#endif //BOUNCINGPLUS_WEAPON_H