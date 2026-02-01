//
// Created by lalit on 1/31/2026.
//

#include "EnemyBehavior.h"
#include "../Enemy.h"
#include "../../../game/Game.h"

EnemyBehavior::EnemyBehavior(Enemy& Owner, Game& game)
{
    this->Owner = &Owner;
    this->game = &game;
}

void EnemyBehavior::Update()
{

}
