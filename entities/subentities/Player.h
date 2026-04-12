//
// Created by lalit on 8/27/2025.
//

#ifndef BOUNCINGPLUS_PLAYER_H
#define BOUNCINGPLUS_PLAYER_H
#include "PlayerLogicProcessor.h"
#include "../Entity.h"
#include "../Weapons.h"
#include "../Powerups.h"

struct Vector2;

class Player : public Entity {

    bool SystemsInitialized = false;

    float Alpha = 1;
    float OrigSpeed;
    Vector2 LastPos = Vector2(0, 0);

    double LastMovedTime;
    int LastKills;

    float IntervalHealth;
    double LastInterval;
    double LastWarningSign;

    double LastTanked;

    public:

        float StressLevel = 0;
        float FrameStressLevel = 0;
        int EnemiesDetected = 0;

        bool isInvincible;
        float ExtraSpeed;
        int ShaderUniformLoc;
        float SpeedBuff;
        bool WarningSign;
        bool HealthConcern;
        double PlayerFrozenTimer = 0;
        bool ReduceSpeedBuff = false;
        PlayerLogicProcessor LogicProcessor;
        WeaponsSystem MainWeaponsSystem;
        PowerupSystem MainPowerupSystem;
        double InvincibilityResetTimer;
        int Kills;
        bool IsPreparingForDash = false;
        bool Dodging =false;

        Player(float X, float Y, float Speed, Texture2D &PlayerTexture, Game &game);
        Player();
        virtual ~Player();
        void PhysicsUpdate(float DeltaTime, double Time);
        void Update();
        void OnDeath() override;
        void OnDelete() override;
        void OnWallVelocityBump(float Power);
        void ToggleInvincibility();

};


#endif //BOUNCINGPLUS_PLAYER_H