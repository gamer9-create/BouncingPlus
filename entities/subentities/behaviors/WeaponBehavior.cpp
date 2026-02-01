//
// Created by lalit on 1/31/2026.
//

#include "WeaponBehavior.h"
#include "../../../game/Game.h"

WeaponBehavior::WeaponBehavior(Enemy& Owner, Game& game) : EnemyBehavior(Owner,game)
{
}

void WeaponBehavior::Update()
{
    float center_x = Owner->BoundingBox.x + (Owner->BoundingBox.width / 2);
    float center_y = Owner->BoundingBox.y + (Owner->BoundingBox.height / 2);

    if (Owner->AngeredRangeBypassTimer > 0)
        Owner->AngeredRangeBypassTimer -= game->GetGameDeltaTime();

    if (Owner->AngeredRangeBypassTimer <= 0)
        Owner->AngeredRangeBypassTimer = 0;
    if (Owner->weaponsSystem.CurrentWeapon != nullptr && Owner->weaponsSystem.TimeStartedReloading == -1 && Owner->weaponsSystem.CurrentWeapon->Ammo > 0 && Owner->weaponsSystem.WeaponAmmo[Owner->weaponsSystem.CurrentWeaponIndex] <= 0)
        Owner->weaponsSystem.Reload();

    Owner->Movement = Vector2(0, 0);
    float plr_center_x = game->MainPlayer->BoundingBox.x + (game->MainPlayer->BoundingBox.width / 2);
    float plr_center_y = game->MainPlayer->BoundingBox.y + (game->MainPlayer->BoundingBox.height / 2);

    float distance = std::sqrt(std::pow(plr_center_x - center_x, 2) + std::pow(plr_center_y - center_y, 2));

    if ((distance <= 800 && (distance <= 36 || game->RayCast({center_x, center_y}, {plr_center_x, plr_center_y}))) || Owner->AngeredRangeBypassTimer > 0.0f) {
        if (distance >= 100) {
            Owner->Movement.x += -(plr_center_x - center_x) / distance * Owner->Speed * (Owner->weaponsSystem.CurrentWeapon->isMelee ? -1 : 1);
            Owner->Movement.y += -(plr_center_y - center_y) / distance * Owner->Speed * (Owner->weaponsSystem.CurrentWeapon->isMelee ? -1 : 1);
        }
        Owner->weaponsSystem.Attack(Vector2(plr_center_x, plr_center_y));
    } else if (Owner->WanderingEnabled) {
        Owner->Wander();
    }

    Owner->MoveAwayFromWalls();
    EnemyBehavior::Update();
}
