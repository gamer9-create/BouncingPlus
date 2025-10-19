//
// Created by lalit on 9/13/2025.
//

#ifndef BOUNCINGPLUS_WEAPON_H
#define BOUNCINGPLUS_WEAPON_H

#include <vector>
#include <string>
#include "raylib.h"

using namespace std;

struct Weapon {
    bool isMelee = false;
    bool ShakeScreen = false;
    bool SlowdownOverTime = false;
    float Speed = 0;
    float Size = 1;
    float Damage = 0;
    float Cooldown = 0;
    float AngleRange = 0;
    float Range = 0;
    int Bullets = -1;
    float Intensity = 0;
    std::string texture;
    std::string sound;
};

class WeaponsSystem {
    public:
        Game* game;
        weak_ptr<Entity> OwnerPtr;
        std::string Weapons[3];
        float AttackCooldowns[3];
        Weapon* CurrentWeapon;
        int CurrentWeaponIndex;

        double PointRemovalTimer;

        std::vector<Vector2> points;

        bool MeleeAnim;
        Texture* MeleeAnimTexture;
        float MeleeAnimAngle;
        float MeleeAnimRange;
        float MeleeAnimPercent;
        float MeleeAnimAlpha;

        WeaponsSystem(shared_ptr<Entity> Owner, Game &game);
        WeaponsSystem();
        virtual ~WeaponsSystem();

        bool Raycast(Rectangle target);
        virtual void Equip(int Index);
        virtual void Unequip();
        virtual void Update();
        virtual void Attack(Vector2 Target);
};

#endif //BOUNCINGPLUS_WEAPON_H