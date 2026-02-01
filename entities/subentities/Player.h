//
// Created by lalit on 8/27/2025.
//

#ifndef BOUNCINGPLUS_PLAYER_H
#define BOUNCINGPLUS_PLAYER_H
#include "Enemy.h"
#include "../Entity.h"
#include "../Weapons.h"
#include "../Powerups.h"


class Player : public Entity {
    void AttackDashedEnemy(std::shared_ptr<Enemy> entity, bool already_attacked);
    void DashLogic();

    bool weaponsSystemInit = false;
    double PlayerFrozenTimer = 0;
    float PlayerDashLineThickness = 10;
    int ShaderUniformLoc;
    float DashCooldown = 0;
    double DashTimeStart = 0;
    float Alpha = 1;
    std::vector<std::weak_ptr<Enemy>> DashedEnemies;
    float OrigSpeed;
    float ExtraSpeed;
    double LastMovedTime;
    int LastKills;

    float IntervalHealth;
    double LastInterval;
    double LastWarningSign;
    bool WarningSign;
    bool HealthConcern;

    public:
        bool isInvincible;
        float PrevHealthBeforeDodge = 100;
        double DodgeHealthResetTimer = -1;
        double LastDodgeHealthResetTimer = -1;
        WeaponsSystem weaponsSystem;
        PowerupSystem powerupSystem;
        int Kills;
        bool IsPreparingForDash = false;
        bool Dodging =false;

        Player(float X, float Y, float Speed, Texture2D &PlayerTexture, Game &game);
        Player();
        virtual ~Player();
        void PhysicsUpdate(float dt);
        void Update();
        void OnWallVelocityBump(float Power);
        void ToggleInvincibility();
};


#endif //BOUNCINGPLUS_PLAYER_H