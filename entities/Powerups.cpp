//
// Created by lalit on 1/23/2026.
//

#include "Powerups.h"

#include <iostream>

#include ".././game/Game.h"
#include "subentities/Player.h"

// TODO: add more powerups

void Powerup::complete(std::shared_ptr<Player> Owner)
{
}

void Powerup::undo(std::shared_ptr<Player> Owner)
{
}

void SpeedPowerup::complete(std::shared_ptr<Player> Owner)
{
    Owner->SpeedBuff += 250.0f * Owner->game->GetGameDeltaTime();
}

void SpeedPowerup::undo(std::shared_ptr<Player> Owner)
{
    Owner->SpeedBuff -= 240.0f * Length;
}

SpeedPowerup::SpeedPowerup()
{
    Cooldown = 12;
    Length = 5;
    Name = "Speed";
    Texture = "speed_powerup_icon";
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
