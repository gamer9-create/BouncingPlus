//
// Created by lalit on 8/27/2025.
//

#include "Player.h"

#include <cfloat>
#include <iostream>
#include <raymath.h>
#include <nlohmann/json.hpp>
#include "Enemy.h"
#include "../../game/Game.h"

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
    this->PrevHealthBeforeDodge = 0;
    this->PlayerDashLineThickness = 10;
    this->ShaderUniformLoc = GetShaderLocation(game.Shaders["dash_arrow"], "time");
}

Player::Player() {
}

Player::~Player() {

}

void Player::ToggleInvincibility() {
    this->isInvincible = !this->isInvincible;
    if (isInvincible) {
        PrevHealthBeforeDodge = Health;
        Health = FLT_MAX;
        isInvincible = true;
    } else {
        Health = PrevHealthBeforeDodge;
        isInvincible = false;
    }
}

void Player::PhysicsUpdate(float dt) {
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
    if (IsMouseButtonPressed(0) && PlayerFrozenTimer > 0 && PlayerFrozenTimer <= 0.9f) {
        PlayerFrozenTimer = 0;
        DodgeHealthResetTimer = 0;
        VelocityPower = 0;
    }
    Movement = Vector2(MovementX, MovementY);
    if (Vector2Distance({0,0},Movement) > 0) {
        LastMovedTime = GetTime();
        ExtraSpeed += 10 * dt;
    }
    if (GetTime() - LastMovedTime > 1)
        ExtraSpeed = 0;
    ExtraSpeed = min(ExtraSpeed, 400.0f);
    if ((Health/MaxHealth) > 2.0f)
        Speed = (OrigSpeed + ExtraSpeed) * (1.0f - min(((Health/MaxHealth)-2.0f) / 2.0f, 0.5f));
    else
        Speed = (OrigSpeed + ExtraSpeed);
    Entity::PhysicsUpdate(dt);
}

void Player::AttackDashedEnemy(std::shared_ptr<Enemy> entity, bool already_attacked) {
    // check if we're colliding with them. if so, attack!
    if (CheckCollisionRecs(BoundingBox, entity->BoundingBox) && !already_attacked) {
        // calculate damage & attack
        float Damage = VelocityPower / 16.0f;
        Damage *= min(max((Health / MaxHealth)-2.0f, 1.0f), 1.5f);
        if (entity->Armor <= 0)
            entity->Health -= Damage;
        else
            entity->Armor -= Damage;
        Health += Damage / 3.0f;

        float amount = 1500.0f;

        // did we kill them? if so, give health & kills
        if (entity->Health <= 0) {
            Health += Damage * 0.45f;
            //game->Slowdown(0.35f, VelocityPower / 1450.0f);
            amount= 950;
            Kills+=1;
        }

        game->MainCameraManager.CameraPosition += Vector2Normalize({(float)GetRandomValue(-50, 50), (float)GetRandomValue(-50, 50)}) * (VelocityPower / 150);
        game->MainCameraManager.ShakeCamera(VelocityPower / (amount - 50));
        game->MainSoundManager.PlaySoundM("dash_hit",min(max(VelocityPower/amount, 0.0f), 1.0f));

        // give them pushback force
        entity->VelocityMovement = VelocityMovement;
        entity->VelocityPower = -VelocityPower/2;

        // increase velocity and mark enemy as attacked
        VelocityPower += VelocityPower / (amount/300);
        DashedEnemies.push_back(std::weak_ptr(entity));
    }
}

void Player::DashLogic() {
    // update dash cooldownm
    if (DashCooldown > 0)
        DashCooldown -= GetFrameTime();

    if (VelocityPower > 0 && DodgeHealthResetTimer <= 0) {
        // get enemies list
        std::vector<shared_ptr<Entity>>* array = &game->MainEntityManager.Entities[EnemyType];
        for (int i = 0; i < array->size(); i++) {
            if (shared_ptr<Enemy> entity = dynamic_pointer_cast<Enemy>(array->at(i)); entity != nullptr and !entity->ShouldDelete) {
                // have we already attacked them? if so, ignore this!!!
                bool already_attacked = false;
                for (auto e : DashedEnemies) {
                    if (!e.owner_before(entity) && !entity.owner_before(e)) {
                        already_attacked = true;
                        break;
                    }
                }

                // attack them
                AttackDashedEnemy(entity, already_attacked);
            }
        }
    } else {
        DashedEnemies.clear();
    }

    auto WorldMousePos = Vector2(static_cast<float> (GetMouseX()) + game->MainCameraManager.CameraPosition.x, static_cast<float> (GetMouseY()) + game->MainCameraManager.CameraPosition.y);
    if (DashCooldown <= 0 && IsKeyDown(KEY_LEFT_SHIFT)) {
        if (!IsPreparingForDash) {
            DashTimeStart = GetTime();
            PlayerDashLineThickness = 10;
        }
        IsPreparingForDash = true;
    }
    if (IsPreparingForDash && !IsKeyDown(KEY_LEFT_SHIFT)) {
        IsPreparingForDash = false;
    }
    if (game->MainPlayer->IsPreparingForDash || game->MainPlayer->weaponsSystem.TimeStartedReloading != -1) {
        game->MainCameraManager.QuickZoom(1.25f, 0.1f);
        if (game->MainPlayer->IsPreparingForDash)
        {
            if (static_cast<float>(GetTime() - DashTimeStart) / 1.1f > 0.8f)
                PlayerDashLineThickness = Lerp(PlayerDashLineThickness, 20, 2 * GetFrameTime());
            float alpha = static_cast<float>(GetTime() - DashTimeStart) / 1.1f;
            Vector2 Target ={(float)GetMouseX(), (float)GetMouseY()};
            float cx = BoundingBox.x + BoundingBox.width / 2;
            float cy = BoundingBox.y + BoundingBox.height / 2;
            float FinalAngle = (atan2(cy - (Target.y + game->MainCameraManager.CameraPosition.y), cx - (Target.x + game->MainCameraManager.CameraPosition.x)) * RAD2DEG);
            Texture2D& MeleeAnimTexture = game->Textures["arrow"];
            float width = MeleeAnimTexture.width;
            float height = MeleeAnimTexture.height;
            BeginShaderMode(game->Shaders["dash_arrow"]);
            float t = static_cast<float>(GetTime() - DashTimeStart);
            SetShaderValue(game->Shaders["dash_arrow"],
                ShaderUniformLoc,
                &t,
                SHADER_ATTRIB_FLOAT);
            float size = 0.375f;
            DrawTexturePro(MeleeAnimTexture, Rectangle(0, 0, width, height *6),
                           Rectangle(BoundingBox.x + BoundingBox.width/2 - game->MainCameraManager.CameraPosition.x - cosf((FinalAngle) * DEG2RAD)*10,
                               BoundingBox.y +BoundingBox.height/2 - game->MainCameraManager.CameraPosition.y - sinf((FinalAngle) * DEG2RAD)*10, width*size,
                                     height * 6 * size), Vector2(width * size * 0.5, height*6*size), FinalAngle-90, ColorAlpha(WHITE, alpha));
            EndShaderMode();
        }
    }
    if ((IsMouseButtonDown(1) || IsMouseButtonDown(0)) && IsPreparingForDash && Health > 0) {
        DashCooldown = 1.5f;
        DashedEnemies.clear();
        VelocityMovement = Vector2Subtract(WorldMousePos, {BoundingBox.x, BoundingBox.y});
        VelocityPower = 1200.0f * max(min(static_cast<float>(GetTime() - DashTimeStart), 1.1f), 0.35f);
        VelocityPower /= min(max((Health / MaxHealth)-2.0f, 1.0f), 1.5f);
        game->MainSoundManager.PlaySoundM("dash");
        PlayerFrozenTimer = 1.0f;
        if (IsMouseButtonDown(0) && !isInvincible) {
            ToggleInvincibility();
            Dodging = true;
            DodgeHealthResetTimer = 1.0f;
        }
        IsPreparingForDash = false;
    }

    // display dashing bar
    int w = 56;
    int h = 15;
    float a = 0.25f;
    if (IsPreparingForDash)
        a = Lerp(0.25f, 0.5f, min( (float)(GetTime() - DashTimeStart) / 0.2f, 1.0f ));
    DrawRectangle((int)(BoundingBox.x + (BoundingBox.width / 2) - (w/2) - game->MainCameraManager.CameraPosition.x),
        (int)(BoundingBox.y + BoundingBox.width + 10 - game->MainCameraManager.CameraPosition.y),
        w, h, ColorAlpha(BLACK, a));
    DrawRectangle((int)(BoundingBox.x + (BoundingBox.width / 2) - (w/2) - game->MainCameraManager.CameraPosition.x)+5,
        (int)(BoundingBox.y + BoundingBox.width + 10 - game->MainCameraManager.CameraPosition.y)+5,
        (!IsPreparingForDash ? 0 : min(static_cast<float>(GetTime() - DashTimeStart) / 1.1f, 1.0f))*(w-10), h-10, ColorAlpha(WHITE, a+0.25f));
}

void Player::Update() {

    // is the weapon system not initialized?? init it now!!!
    if (!this->weaponsSystemInit) {
        this->weaponsSystem = WeaponsSystem(shared_from_this(), *game);
        auto f = game->LevelData[game->CurrentLevelName]["inventory"];
        for (int i = 0; i < f.size(); i++) {
            this->weaponsSystem.Weapons[i] = f[i];
            this->weaponsSystem.WeaponAmmo[i] = game->Weapons[f[i]].Ammo;
        }
        this->weaponsSystem.Equip(0);
        this->weaponsSystemInit = true;
    }

    // player transparency processing
    EntityColor = ColorAlpha(WHITE, Alpha);
    Alpha = Lerp(Alpha, (DodgeHealthResetTimer > 0 ? 0.5f : 1.0f), 5.5f*GetFrameTime());

    // dashing logic
    DashLogic();

    // health cap + dodging stuff
    if (DodgeHealthResetTimer > 0) {
        DodgeHealthResetTimer -= GetFrameTime();
        if (DodgeHealthResetTimer < 0)
            Dodging = false;
    }

    // dodge health reset
    if (DodgeHealthResetTimer <= 0 && isInvincible && Dodging) {
        ToggleInvincibility();
    }

    if (PlayerFrozenTimer <= 0) {
        // firing logic
        if (IsMouseButtonDown(0) && weaponsSystem.CurrentWeapon != nullptr &&
            weaponsSystem.CurrentWeapon->Ammo > 0 && weaponsSystem.WeaponAmmo[weaponsSystem.CurrentWeaponIndex] <=0 &&
            weaponsSystem.AttackCooldowns[weaponsSystem.CurrentWeaponIndex] >= weaponsSystem.CurrentWeapon->Cooldown)
        {
            weaponsSystem.Reload();
        }

        auto WorldMousePos = Vector2(static_cast<float> (GetMouseX()) + game->MainCameraManager.CameraPosition.x, static_cast<float> (GetMouseY()) + game->MainCameraManager.CameraPosition.y);
        if (IsMouseButtonDown(0) && !IsPreparingForDash)
            weaponsSystem.Attack(WorldMousePos);

        // reload logic
        if (IsKeyPressed(KEY_R) && weaponsSystem.TimeStartedReloading == -1)
            weaponsSystem.Reload();

        //dropping stuff
        if (IsKeyPressed(KEY_Q) && weaponsSystem.CurrentWeaponIndex != -1)
            weaponsSystem.DropWeapon(weaponsSystem.Weapons[weaponsSystem.CurrentWeaponIndex]);

        // inventory input logic
        if (IsKeyPressed(KEY_ONE)) {
            if (weaponsSystem.CurrentWeaponIndex != 0) {
                weaponsSystem.Equip(0);
            } else if (weaponsSystem.CurrentWeaponIndex == 0) {
                weaponsSystem.Unequip();
            }
        }
        if (IsKeyPressed(KEY_TWO)) {
            if (weaponsSystem.CurrentWeaponIndex != 1) {
                weaponsSystem.Equip(1);
            } else if (weaponsSystem.CurrentWeaponIndex == 1) {
                weaponsSystem.Unequip();
            }
        }
        if (IsKeyPressed(KEY_THREE)) {
            if (weaponsSystem.CurrentWeaponIndex != 2) {
                weaponsSystem.Equip(2);
            } else if (weaponsSystem.CurrentWeaponIndex == 2) {
                weaponsSystem.Unequip();
            }
        }
    } else {
        PlayerFrozenTimer -= GetFrameTime();
    }

    // update entity
    Entity::Update();
    weaponsSystem.Update();

    // did we get a kill? play kill sound game!
    if (Kills != LastKills) {
        game->MainSoundManager.PlaySoundM("death");
    }
    LastKills = Kills;

}
