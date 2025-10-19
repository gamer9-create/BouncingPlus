//
// Created by lalit on 8/27/2025.
//

#include "Player.h"

#include <iostream>
#include <ostream>
#include <raymath.h>

#include "Enemy.h"
#include "Game.h"

using namespace std;

Player::Player(float X, float Y, float Speed, Texture2D &PlayerTexture, Game &game) : Entity(PlayerTexture,
                                                                   Rectangle(X - 18, Y - 18, 36, 36), Speed, game) {
    this->Type = PlayerType;
    this->Kills = 0;
    this->LastKills = 0;
    this->OrigSpeed = Speed;

}

Player::Player() {
}

Player::~Player() {

}

void Player::PhysicsUpdate(float dt) {
    float MovementX = 0;
    float MovementY = 0;
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
    Movement = Vector2(MovementX, MovementY);
    if (Vector2Distance({0,0},Movement) > 0) {
        LastMovedTime = GetTime();
        if (ExtraSpeed < 400)
            ExtraSpeed += 10 * GetFrameTime();
        else
            ExtraSpeed = 400;
    }
    if (GetTime() - LastMovedTime > 1) {
        ExtraSpeed = 0;
    }
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
        if (entity->Armor <= 0)
            entity->Health -= Damage;
        else
            entity->Armor -= Damage;
        Health += Damage / 8.0f;

        // play sound
        SetSoundVolume(game->Sounds["dash_hit"], min(max(VelocityPower / 1500.0f, 0.0f), 1.0f));
        PlaySound(game->Sounds["dash_hit"]);

        // did we kill them? if so, give health & kills
        if (entity->Health <= 0) {
            Health += VelocityPower / 18.0f;
            Kills+=1;
        }

        // give them pushback force
        entity->VelocityMovement = VelocityMovement;
        entity->VelocityPower = -VelocityPower;

        // shake camera
        game->ShakeCamera(VelocityPower / 1450.0f);
        game->CameraPosition += Vector2Normalize({(float)GetRandomValue(-50, 50), (float)GetRandomValue(-50, 50)}) * (VelocityPower / 10.0f);

        // increase velocity and mark enemy as attacked
        VelocityPower += VelocityPower / 5.0f;
        DashedEnemies.push_back(std::weak_ptr(entity));
    }
}

void Player::DashLogic() {
    // update dash cooldownm
    if (DashCooldown > 0)
        DashCooldown -= GetFrameTime();

    if (VelocityPower > 0) {
        // get enemies list
        std::vector<shared_ptr<Entity>>* array = &game->Entities[EnemyType];
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

    auto WorldMousePos = Vector2(static_cast<float> (GetMouseX()) + game->CameraPosition.x, static_cast<float> (GetMouseY()) + game->CameraPosition.y);
    if (DashCooldown <= 0 && IsKeyDown(KEY_LEFT_SHIFT)) {
        if (!IsDashing)
            DashTimeStart = GetTime();
        IsDashing = true;
        DrawCircle(0,0,10,RED);
    }
    if (IsDashing && !IsKeyDown(KEY_LEFT_SHIFT)) {
        IsDashing = false;
    }
    if (IsMouseButtonDown(1) && IsDashing) {
        DashCooldown = 1.5f;
        DashedEnemies.clear();
        VelocityMovement = Vector2Subtract(WorldMousePos, {BoundingBox.x, BoundingBox.y});
        VelocityPower = 1200.0f * max(min(static_cast<float>(GetTime() - DashTimeStart), 1.1f), 0.35f);
        VelocityPower /= min(max((Health / MaxHealth)-2.0f, 1.0f), 1.5f);
        PlaySound(game->Sounds["dash"]);
        IsDashing = false;
    }

    // display dashing bar
    int w = 56;
    int h = 15;
    float a = 0.25f;
    if (IsDashing)
        a = Lerp(0.25f, 0.5f, min( (float)(GetTime() - DashTimeStart) / 0.2f, 1.0f ));
    DrawRectangle((int)(BoundingBox.x + (BoundingBox.width / 2) - (w/2) - game->CameraPosition.x),
        (int)(BoundingBox.y + BoundingBox.width + 10 - game->CameraPosition.y),
        w, h, ColorAlpha(BLACK, a));
    DrawRectangle((int)(BoundingBox.x + (BoundingBox.width / 2) - (w/2) - game->CameraPosition.x)+5,
        (int)(BoundingBox.y + BoundingBox.width + 10 - game->CameraPosition.y)+5,
        (!IsDashing ? 0 : min(static_cast<float>(GetTime() - DashTimeStart) / 1.1f, 1.0f))*(w-10), h-10, ColorAlpha(WHITE, a));
}

void Player::Update() {

    // is the weapon system not initialized?? init it now!!!
    if (!this->weaponsSystemInit) {
        this->weaponsSystem = WeaponsSystem(shared_from_this(), *game);
        this->weaponsSystem.Weapons[0] = "Player Gun";
        this->weaponsSystem.Weapons[1] = "Shotgun";
        this->weaponsSystem.Weapons[2] = "Sword";
        this->weaponsSystem.Equip(0);
        this->weaponsSystemInit = true;
    }

    // health cap
    if (Health > 400)
        Health = 400;

    // dashing logic
    DashLogic();

    // firing logic
    auto WorldMousePos = Vector2(static_cast<float> (GetMouseX()) + game->CameraPosition.x, static_cast<float> (GetMouseY()) + game->CameraPosition.y);
    if (IsMouseButtonDown(0) && !IsDashing)
        weaponsSystem.Attack(WorldMousePos);

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

    // update entitty
    Entity::Update();
    weaponsSystem.Update();

    // did we get a kill? play kill sound!
    if (Kills != LastKills)
        PlaySound(game->Sounds["death"]);
    LastKills = Kills;

}
