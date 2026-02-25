//
// Created by lalit on 10/28/2025.
//

#include "FaceBoss.h"

#include <raymath.h>

#include "../../game/Game.h"
#include "../subentities/Spawner.h"

FaceBoss::FaceBoss() {
}

FaceBoss::FaceBoss(Game &game, float bbox_x, float bbox_y) :Entity(game.Textures["boss1_img"], {bbox_x, bbox_y, 112*4, 71*4}, 0, game) {
    // Exact starting position of the boss
    StartPos = Vector2(bbox_x+BoundingBox.width/2, bbox_y+BoundingBox.height/2);

    // Tracking the bossfight progress
    BossFightStarted = false;

    // Setting entity type to boss
    this->Type = BossType;
}

void FaceBoss::Render() {

    // Rendering of eyes
    Vector2 Eye1Position = {StartPos.x - 75, StartPos.y-75};
    Vector2 Eye2Position = {StartPos.x + 75, StartPos.y-75};

    Vector2 Offset = Vector2Multiply(Vector2Normalize(Vector2Subtract(StartPos, {
        this->game->MainPlayer->BoundingBox.x + this->game->MainPlayer->BoundingBox.width/2,
        this->game->MainPlayer->BoundingBox.y + this->game->MainPlayer->BoundingBox.width/2
    })), {-25,-25});

    DrawTexturePro(game->Textures["enemy"], {0, 0, 36, 36},
            {Eye1Position.x + Offset.x, Eye1Position.y + Offset.y, 36, 36},
            {18, 18}, 0, BLUE);

    Offset = Vector2Multiply(Vector2Normalize(Vector2Subtract(StartPos, {
        this->game->MainPlayer->BoundingBox.x + this->game->MainPlayer->BoundingBox.width/2,
        this->game->MainPlayer->BoundingBox.y + this->game->MainPlayer->BoundingBox.width/2
    })), {-25,-25});

    DrawTexturePro(game->Textures["enemy"], {0, 0, 36, 36},
            {Eye2Position.x + Offset.x, Eye2Position.y + Offset.y, 36, 36},
            {18, 18}, 0, BLUE);


    // Mouth rendering
    DrawTexturePro(game->Textures["mouth"], {0, 0, 300, 36},
            {StartPos.x+(float)cos(game->GetGameTime()) * 5, StartPos.y+60+(float)sin(game->GetGameTime()) * 5, 300, 36},
            {150, 18}, 0, BLUE);


}

FaceBoss::~FaceBoss() {
}

void FaceBoss::Update() {
    // Player boss trigger
    float Distance = Vector2Distance({this->game->MainPlayer->BoundingBox.x + this->game->MainPlayer->BoundingBox.width/2,
        this->game->MainPlayer->BoundingBox.y + this->game->MainPlayer->BoundingBox.width/2}, StartPos);
    if (Distance <= 450 && Health > 0) {
        if (!BossFightStarted) {
            game->MainSoundManager.PlaySoundM("badtime");
            this->game->MainGameModeManager.LevelTimer = 300;
        }
        BossFightStarted = true;
        this->game->MainGameModeManager.CurrentBoss = this;
        this->game->MainGameModeManager.CurrentBossName = "The Bouncing Face";
    }

    // Stops bossfight if boss is dead
    if (Health <= 0)
        BossFightStarted = false;

    // Forces spawners to be active
    if (BossFightStarted) {
        std::vector<shared_ptr<Entity>> array = game->MainEntityManager.Entities[SpawnerType];
        for (int i = 0; i < array.size(); i++) {
            if (shared_ptr<Spawner> entity = dynamic_pointer_cast<Spawner>(array.at(i)); entity != nullptr and !entity->ShouldDelete) {
                entity->SpawnerIsActive = this->game->MainGameModeManager.LevelTimer > 0 ? 999 : 0;
            }
        }
    }
    Entity::Update();
    Render();
}
