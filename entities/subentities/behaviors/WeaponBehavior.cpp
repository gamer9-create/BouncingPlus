//
// Created by lalit on 1/31/2026.
//

#include "WeaponBehavior.h"

#include <iostream>

#include "../../../game/Game.h"
#include <raymath.h>

WeaponBehavior::WeaponBehavior()
{
}

WeaponBehavior::~WeaponBehavior()
{
}

WeaponBehavior::WeaponBehavior(Enemy& Owner, Game& game) : EnemyBehavior(Owner,game)
{
    BehaviorType = WeaponBehaviorType;
    CoverRefreshTimer = 0;
    CoverPosition = {0,0};
    FoundCover = false;
    CoverSearching = false;
    Target = Owner.GetCenter();
}

void WeaponBehavior::MoveForCover()
{
    float center_x = Owner->BoundingBox.x + (Owner->BoundingBox.width / 2);
    float center_y = Owner->BoundingBox.y + (Owner->BoundingBox.height / 2);

    float plr_center_x = game->MainPlayer->BoundingBox.x + (game->MainPlayer->BoundingBox.width / 2);
    float plr_center_y = game->MainPlayer->BoundingBox.y + (game->MainPlayer->BoundingBox.height / 2);

    if (game->GetGameTime() - CoverRefreshTimer >= 0.5f)
    {
        FoundCover = false;
        float AngleToPlayer = 180 - Vector2LineAngle({center_x,center_x},{plr_center_x,plr_center_y}) * RAD2DEG;
        for (int i = 0; i < 60; i++)
        {
            float Angle = i * 6.0f;
            if (abs(Angle - AngleToPlayer) <= 120)
                continue;
            float X = cos(Angle * (2 * PI / 360))*2000;
            float Y = sin(Angle * (2 * PI / 360))*2000;
            RayCastData d = game->RayCastPoint({center_x,center_y}, {center_x + X,center_y + Y});
            if ((!d.HitAir) || Vector2Distance({plr_center_x,plr_center_y}, d.HitPosition) >= 500) // if we hit wall?
            {
                RayCastData p = game->RayCastPoint({plr_center_x,plr_center_y}, d.HitPosition);
                if (Vector2Distance(p.HitPosition, d.HitPosition) >= 150 && !p.HitAir && Vector2Distance({plr_center_x,plr_center_y}, d.HitPosition) >= 150)
                {
                    CoverPosition = d.HitPosition;
                    FoundCover = true;
                    break;
                }
            }
        }
        CoverRefreshTimer = game->GetGameTime();
    }
    if (FoundCover)
    {
        Owner->Movement = Vector2Normalize(Vector2Subtract(CoverPosition, {center_x,center_y}));
    } else
    {
        Owner->Movement = Vector2Normalize(Vector2Subtract({center_x,center_y},{plr_center_x,plr_center_y}));
    }

    Owner->Health += Owner->HealthRegenRate * game->GetGameDeltaTime();
}

void WeaponBehavior::Update()
{

    Owner->Movement = Vector2{0,0};

    // programming is so fun and awesome!!!!!!!!

    float center_x = Owner->BoundingBox.x + (Owner->BoundingBox.width / 2);
    float center_y = Owner->BoundingBox.y + (Owner->BoundingBox.height / 2);

    // checks for enemy anger
    if (Owner->AngeredRangeBypassTimer > 0)
        Owner->AngeredRangeBypassTimer -= game->GetGameDeltaTime();

    if (Owner->AngeredRangeBypassTimer <= 0)
        Owner->AngeredRangeBypassTimer = 0;
    if (Owner->weaponsSystem.CurrentWeapon != nullptr && Owner->weaponsSystem.TimeStartedReloading == -1 && Owner->weaponsSystem.CurrentWeapon->Ammo > 0 && Owner->weaponsSystem.WeaponAmmo[Owner->weaponsSystem.CurrentWeaponIndex] <= 0)
        Owner->weaponsSystem.Reload();

    // distance/firing checks
    float plr_center_x = game->MainPlayer->BoundingBox.x + (game->MainPlayer->BoundingBox.width / 2);
    float plr_center_y = game->MainPlayer->BoundingBox.y + (game->MainPlayer->BoundingBox.height / 2);

    float distance = std::sqrt(std::pow(plr_center_x - center_x, 2) + std::pow(plr_center_y - center_y, 2));

    bool Attacking = false;

    CoverSearching = Owner->RemainingHealthOfOriginalHealth <= 0.6f;
    if ((distance <= 800 && (distance <= 36 || game->RayCast({center_x, center_y}, {plr_center_x, plr_center_y}))) || Owner->AngeredRangeBypassTimer > 0.0f) {
        if (distance >= 100 && !CoverSearching) {
            Vector2 othermov = {0,0};
            othermov.x += -(plr_center_x - center_x) / distance * Owner->Speed * (Owner->weaponsSystem.CurrentWeapon != nullptr ? (Owner->weaponsSystem.CurrentWeapon->isMelee ? -1.0f : 1.0f) : 1.0f);
            othermov.y += -(plr_center_y - center_y) / distance * Owner->Speed * (Owner->weaponsSystem.CurrentWeapon != nullptr ? (Owner->weaponsSystem.CurrentWeapon->isMelee ? -1.0f : 1.0f) : 1.0f);
            Owner->MoveAwayFromWalls();
            Owner->Movement=Vector2Lerp(othermov, Owner->Movement,0.5f);
        }

        Target = Vector2{plr_center_x, plr_center_y};
        if (game->MainPlayer->Speed >= 200 && (Owner->weaponsSystem.CurrentWeapon != nullptr ? !Owner->weaponsSystem.CurrentWeapon->isMelee : true))
        {
            Target = Vector2Add(Target, Vector2Multiply(Vector2Normalize(game->MainPlayer->Movement), {game->MainPlayer->Speed * 0.2f,game->MainPlayer->Speed * 0.2f}));
        }

        game->MainPlayer->EnemiesDetected += 1;

        Attacking = true;
        Owner->weaponsSystem.Attack(Target);
    } else if (Owner->WanderingEnabled && !CoverSearching) {
        Owner->Wander(); // enemy wandering
        Target = Owner->GetCenter() + Owner->Movement;
    }

    if (CoverSearching) {
        MoveForCover();
        if (!Attacking)
            Target = Owner->GetCenter() + Owner->Movement;
    }

    EnemyBehavior::Update();
}
