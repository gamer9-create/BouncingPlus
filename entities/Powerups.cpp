//
// Created by lalit on 1/23/2026.
//

#include "Powerups.h"

#include <iostream>

#include ".././game/Game.h"
#include "subentities/Bullet.h"
#include "subentities/Player.h"

#include <raylib.h>
#include <raymath.h>

// TODO: add more powerups

void Powerup::complete(std::shared_ptr<Player> Owner)
{
}

void Powerup::undo(std::shared_ptr<Player> Owner)
{
}

void SpeedPowerup::complete(std::shared_ptr<Player> Owner)
{
    Powerup::complete(Owner);
    Owner->ReduceSpeedBuff = false;
    Owner->SpeedBuff += (250.0f - (40.0f * Owner->powerupSystem.CurrentLength)) * Owner->game->GetGameDeltaTime();
}

void SpeedPowerup::undo(std::shared_ptr<Player> Owner)
{
    Powerup::undo(Owner);
    Owner->ReduceSpeedBuff = true;
    Owner->SpeedBuff -= 100;
}

SpeedPowerup::SpeedPowerup()
{
    Cooldown = 15;
    Length = 5;
    Name = "Speed";
}

ShieldPowerup::ShieldPowerup()
{
    Cooldown = 30;
    Length = 15;
    Name = "Bullet Shield";
}

void ShieldPowerup::complete(std::shared_ptr<Player> Owner)
{
    Powerup::complete(Owner);

    transBuff = lerp(transBuff, 0.0f, Owner->game->GetGameDeltaTime() * LerpSpeed);
    float circle_transparency = min(Length - Owner->powerupSystem.CurrentLength, DefaultTrans);
    if (Owner->powerupSystem.CurrentLength <= DefaultTrans)
        circle_transparency = Owner->powerupSystem.CurrentLength;
    else
        circle_transparency += transBuff;

    std::vector<shared_ptr<Entity>>* array = &Owner->game->MainEntityManager.Entities[BulletType];
    for (int i = 0; i < array->size(); i++) {
        if (shared_ptr<Bullet> entity = dynamic_pointer_cast<Bullet>(array->at(i)); entity != nullptr && !entity->ShouldDelete) {
            float dist = Vector2Distance({Owner->BoundingBox.x + Owner->BoundingBox.width / 2, Owner->BoundingBox.y + Owner->BoundingBox.height / 2},
                    {entity->BoundingBox.x + entity->BoundingBox.width / 2, entity->BoundingBox.y + entity->BoundingBox.height / 2});
            auto p = entity->OwnerPtr.lock();
            if (dist <= FieldSize && p != Owner)
            {
                entity->Rotation = -(180 - (Vector2LineAngle({Owner->BoundingBox.x + Owner->BoundingBox.width / 2, Owner->BoundingBox.y + Owner->BoundingBox.height / 2},
                    {entity->BoundingBox.x + entity->BoundingBox.width / 2, entity->BoundingBox.y + entity->BoundingBox.height / 2}) * RAD2DEG));

                float X = cos(entity->Rotation * (2 * PI / 360))*100;
                float Y = sin(entity->Rotation * (2 * PI / 360))*100;

                entity->Movement = Vector2(X, Y);

                transBuff = 1.0f - DefaultTrans;
            }
        }
    }

    displayFieldSize = lerp(displayFieldSize,FieldSize,Owner->game->GetGameDeltaTime() * LerpSpeed);
    for (int i = 0; i < ShieldThickness; i++)
    {
        DrawCircleLines(round(Owner->BoundingBox.x + Owner->BoundingBox.width / 2), round(Owner->BoundingBox.y + Owner->BoundingBox.height / 2), displayFieldSize/2 - i, ColorAlpha(YELLOW, circle_transparency));
    }
}

void ShieldPowerup::undo(std::shared_ptr<Player> Owner)
{
    Powerup::undo(Owner);
    displayFieldSize = 0;
    transBuff = 0;
    FieldSize -= 35.0f;
    Cooldown += 5.0f;
    Length -= 5.0f;
    Length = max(Length, 5.0f);
    Cooldown = min(Cooldown, 45.0f);
    FieldSize = max(FieldSize, 150.0f);
}

void PowerupSystem::Activate()
{
    if (CurrentPowerup != nullptr && CurrentCooldown <= 0 && CurrentLength <= 0)
    {
        CurrentCooldown = CurrentPowerup->Cooldown;
        CurrentLength = CurrentPowerup->Length;
        PowerupIsActive = true;
    }
}

void PowerupSystem::SetPowerup(Powerup* Powerup)
{
    if (this->CurrentPowerup != nullptr)
        this->CurrentPowerup->undo(Owner);
    this->CurrentPowerup = Powerup;
    PowerupIsActive = false;
    CurrentCooldown = 0;
    CurrentLength = 0;
}

void PowerupSystem::Update()
{
    if (CurrentPowerup != nullptr)
    {
        if (PowerupIsActive)
        {
            CurrentPowerup->complete(Owner);
            CurrentLength -= game->GetGameDeltaTime();
            if (CurrentLength <= 0)
            {
                CurrentPowerup->undo(Owner);
                PowerupIsActive = false;
                CurrentCooldown = CurrentPowerup->Cooldown;
            }
        } else
        {
            CurrentCooldown -= game->GetGameDeltaTime();
        }
    }
}

PowerupSystem::PowerupSystem(std::shared_ptr<Player> Owner, Game& game)
{
    this->game = &game;
    this->Owner = Owner;
}

PowerupSystem::PowerupSystem()
{
}

PowerupSystem::~PowerupSystem()
{
}
