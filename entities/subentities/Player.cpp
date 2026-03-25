//
// Created by lalit on 8/27/2025.
//

#include "Player.h"
#include <nlohmann/json.hpp>
#include "Enemy.h"
#include "../../game/Game.h"
#include "Bullet.h"
#include "raylib.h"
#include <raymath.h>

using namespace std;

// the player's name is Jones
// little bouncing ballz boy!
Player::Player(float X, float Y, float Speed, Texture2D &PlayerTexture, Game &game) : Entity(PlayerTexture,
                                                                   Rectangle(X - 18, Y - 18, 36, 36), Speed, game) {
    this->Type = PlayerType;
    this->Kills = 0;
    this->isInvincible = false;
    this->LastKills = 0;
    this->OrigSpeed = Speed;
    this->ExtraSpeed = 0;
    this->IntervalHealth = Health;
    this->LastInterval = game.GetGameTime();
    this->LastWarningSign = game.GetGameTime();
    this->HealthConcern = false;
    this->WarningSign = false;
    this->SpeedBuff = 0;
    this->LastTanked = 0;
    this->LastPos = Vector2(0, 0);
    this->InvincibilityResetTimer = 0;
    this->ShaderUniformLoc = GetShaderLocation(game.GameResources.Shaders["dash_arrow"], "time");
}

Player::Player() {
}

Player::~Player() {

}

void Player::ToggleInvincibility() {
    this->isInvincible = !this->isInvincible;
}

void Player::PhysicsUpdate(float DeltaTime, double Time) {
    float MovementX = 0;
    float MovementY = 0;
    if (PlayerFrozenTimer <= 0) {
        if (IsKeyDown(KEY_W)) {
            MovementY -= Speed;
        }
        if (IsKeyDown(KEY_S)) {
            MovementY += Speed;
        }
        if (IsKeyDown(KEY_A)) {
            MovementX -= Speed;
        }
        if (IsKeyDown(KEY_D)) {
            MovementX += Speed;
        }
    }
    if (Health >= 200 && Time - LastTanked >= 2)
    {
        Health -= 2;
        LastTanked = Time;
    }
    if (VelocityPower < 50)
    {
        VelocityPower = 0;
    }
    Movement = Vector2(MovementX, MovementY);
    if (Vector2Distance(LastPos, {BoundingBox.x,BoundingBox.y}) > 0 && Vector2Distance({0,0}, Movement) > 0) {
        LastMovedTime = Time;
        ExtraSpeed += 10 * DeltaTime;
    }
    LastPos = {BoundingBox.x,BoundingBox.y};
    if (Time - LastMovedTime > 1)
        ExtraSpeed = 0;
    if (ReduceSpeedBuff)
    {
        SpeedBuff -= 100 * DeltaTime;
        if (SpeedBuff <= 0)
        {
            SpeedBuff = 0;
            ReduceSpeedBuff = false;
        }
    }
    ExtraSpeed = min(ExtraSpeed, 400.0f);
    Speed = (OrigSpeed + ExtraSpeed + SpeedBuff);
    if ((Health/MaxHealth) > 2.0f)
        Speed *= (1.0f - min(((Health/MaxHealth)-3.0f) / 2.0f, 0.5f));
    LogicProcessor.PhysicsUpdate();
    Entity::PhysicsUpdate(DeltaTime, Time);
}

void Player::OnWallVelocityBump(float Power)
{
    if (Power > 400)
    {
        float PreviousH = Health;
        Entity::OnWallVelocityBump(Power);
        game->GameSounds.PlaySoundM("dash_wall_hit");
        float Damage = Power / 130.0f;
        if (Health - Damage >= 20)
        {
            Health -= Damage;
        } else if (PreviousH >= 20)
        {
            Health = 20;
        }
    }
}


void Player::Update() {

    // is the weapon system not initialized?? init it now!!!
    if (!this->SystemsInitialized) {
        this->LogicProcessor = PlayerLogicProcessor(dynamic_pointer_cast<Player>(shared_from_this()));
        this->MainWeaponsSystem = WeaponsSystem(shared_from_this(), *game);
        this->MainPowerupSystem = PowerupSystem(dynamic_pointer_cast<Player>(shared_from_this()), *game);
        auto f = game->LevelData[game->CurrentLevelName]["player"]["inventory"];
        for (int i = 0; i < (int)min((float)f.size(),3.0f); i++) {
            this->MainWeaponsSystem.Weapons[i] = f[i];
            this->MainWeaponsSystem.WeaponAmmo[i] = game->GameResources.Weapons[f[i]].Ammo;
        }
        if (game->GameResources.Powerups.contains(game->LevelData[game->CurrentLevelName]["player"]["powerup"]))
        {
            MainPowerupSystem.SetPowerup(game->GameResources.Powerups[game->LevelData[game->CurrentLevelName]["player"]["powerup"]]);
        }
        this->MainWeaponsSystem.Equip(0);
        this->SystemsInitialized = true;
    }

    if (Health > 1000)
        Health = 1000;

    // warning sign interval
    if (game->GetGameTime() - LastInterval >= 1.5f)
    {
        HealthConcern = (IntervalHealth - Health) >= 75;
        LastInterval = game->GetGameTime();
    }
    if (Health < 50)
        HealthConcern = true;

    if (game->GetGameTime() - LastWarningSign >= 0.1f)
    {
        WarningSign = !WarningSign;
        LastWarningSign = game->GetGameTime();
    }

    if (Health > 0 && HealthConcern && WarningSign)
    {
        DrawTexturePro(game->GameResources.Textures["warning"], {0,0,33,34},{BoundingBox.x + BoundingBox.width/2 + 12,BoundingBox.y - 24 - 10,24,24},{0,0},0,WHITE);
    }

    // player transparency processing
    EntityColor = ColorAlpha(WHITE, Alpha);
    Alpha = Lerp(Alpha, (InvincibilityResetTimer > 0 ? 0.5f : 1.0f), 5.5f*game->GetGameDeltaTime());

    if (InvincibilityResetTimer > 0)
        InvincibilityResetTimer -= game->GetGameDeltaTime();
    if (InvincibilityResetTimer <= 0)
    {
        isInvincible = false;
        Dodging=false;
    }

    if (PlayerFrozenTimer <= 0) {
        // powerup logic
        if (IsKeyDown(KEY_F))
        {
            MainPowerupSystem.Activate();
        }

        // firing logic
        if (IsMouseButtonDown(0) && MainWeaponsSystem.CurrentWeapon != nullptr &&
            MainWeaponsSystem.CurrentWeapon->Ammo > 0 && MainWeaponsSystem.WeaponAmmo[MainWeaponsSystem.CurrentWeaponIndex] <=0 &&
            MainWeaponsSystem.AttackCooldowns[MainWeaponsSystem.CurrentWeaponIndex] >= MainWeaponsSystem.CurrentWeapon->Cooldown)
        {
            MainWeaponsSystem.Reload();
        }
        //Vector2 WorldMousePos = Vector2{0, 0};
        if ((IsMouseButtonDown(0) || IsKeyDown(KEY_SPACE)) && !IsPreparingForDash)
        {
            MainWeaponsSystem.Attack(GetScreenToWorld2D(GetMousePosition(), game->GameCamera.RaylibCamera));
        }

        // reload logic
        if (IsKeyPressed(KEY_R) && MainWeaponsSystem.TimeStartedReloading == -1)
            MainWeaponsSystem.Reload();

        //dropping stuff
        if (IsKeyPressed(KEY_Q) && MainWeaponsSystem.CurrentWeaponIndex != -1)
            MainWeaponsSystem.DropWeapon(MainWeaponsSystem.Weapons[MainWeaponsSystem.CurrentWeaponIndex]);

        // inventory input logic
        if (IsKeyPressed(KEY_ONE)) {
            if (MainWeaponsSystem.CurrentWeaponIndex != 0) {
                MainWeaponsSystem.Equip(0);
            } else if (MainWeaponsSystem.CurrentWeaponIndex == 0) {
                MainWeaponsSystem.Unequip();
            }
        }
        if (IsKeyPressed(KEY_TWO)) {
            if (MainWeaponsSystem.CurrentWeaponIndex != 1) {
                MainWeaponsSystem.Equip(1);
            } else if (MainWeaponsSystem.CurrentWeaponIndex == 1) {
                MainWeaponsSystem.Unequip();
            }
        }
        if (IsKeyPressed(KEY_THREE)) {
            if (MainWeaponsSystem.CurrentWeaponIndex != 2) {
                MainWeaponsSystem.Equip(2);
            } else if (MainWeaponsSystem.CurrentWeaponIndex == 2) {
                MainWeaponsSystem.Unequip();
            }
        }
    } else {
        PlayerFrozenTimer -= game->GetGameDeltaTime();
    }

    game->GameScore += Health * 0.005f * game->GetGameDeltaTime();

    // update entity
    Entity::Update();
    MainWeaponsSystem.Update();
    MainPowerupSystem.Update();
    LogicProcessor.Update();

    // did we get a kill? play kill sound game!
    if (Kills != LastKills) {
        game->GameSounds.PlaySoundM("death");
        game->GameScore += 100;
    }
    LastKills = Kills;
}

void Player::OnDelete()
{
    MainWeaponsSystem.Unequip();
    LogicProcessor.Destroy();
    Entity::OnDelete();
}
