//
// Created by lalit on 1/23/2026.
//

#include "Powerups.h"
#include ".././game/Game.h"
#include "subentities/Player.h"

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
