//
// Created by lalit on 2/2/2026.
//

#include "CatchBehavior.h"

#include <iostream>
#include <raymath.h>

#include "../Enemy.h"
#include "../../../game/Game.h"

CatchBehavior::CatchBehavior()
{
    ResetDirection();
}

CatchBehavior::CatchBehavior(Enemy& Owner, Game& game) : EnemyBehavior(Owner,game)
{
    ResetDirection();
}

CatchBehavior::~CatchBehavior()
{
}

void CatchBehavior::ResetDirection()
{
    currentDir = Vector2Normalize({GetRandomValue(1,2) == 1 ? -1.0f : 1.0f, GetRandomValue(1,2) == 1 ? -1.0f : 1.0f});
    currentSpeed = GetRandomValue(700, 1200);
}

void CatchBehavior::Update()
{
    Owner->weaponsSystem.Unequip();
    Owner->Speed = currentSpeed;
    Owner->Movement = currentDir;
    Owner->Rotation = 180 - (Vector2LineAngle({Owner->BoundingBox.x, Owner->BoundingBox.y}, lastPosition) * RAD2DEG);

    float speed = Vector2Distance({Owner->BoundingBox.x, Owner->BoundingBox.y}, lastPosition) / (game->GetGameTime() - lastTime);
    if (speed < 150)
        ResetDirection();

    float distance = Vector2Distance({Owner->BoundingBox.x, Owner->BoundingBox.y}, {game->MainPlayer->BoundingBox.x, game->MainPlayer->BoundingBox.y});
    if (distance < 600 && game->GetGameTime() - attackCooldown >= 1.5f)
    {
        currentDir = Vector2Normalize(Vector2Subtract({game->MainPlayer->BoundingBox.x, game->MainPlayer->BoundingBox.y}, {Owner->BoundingBox.x, Owner->BoundingBox.y}));
        if (distance < 100)
        {
            Owner->DamageOther(game->MainPlayer, 10);
            game->MainCameraManager.ShakeCamera(0.15f);
            game->MainSoundManager.PlaySoundM("dash_hit", 0.1f);
            attackCooldown = game->GetGameTime();
        }

    }

    EnemyBehavior::Update();
    lastPosition = {Owner->BoundingBox.x, Owner->BoundingBox.y};
    lastTime = game->GetGameTime();
}
