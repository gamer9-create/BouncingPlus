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
    StartPos = Vector2(bbox_x+BoundingBox.width/2, bbox_y+BoundingBox.height/2);
    BossFightStarted = false;
    this->Type = BossType;
}

void FaceBoss::Render() {

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


    DrawTexturePro(game->Textures["mouth"], {0, 0, 300, 36},
            {StartPos.x+(float)cos(GetTime()) * 5, StartPos.y+60+(float)sin(GetTime()) * 5, 300, 36},
            {150, 18}, 0, BLUE);


}

FaceBoss::~FaceBoss() {
}

void FaceBoss::Update() {
    float Distance = Vector2Distance({this->game->MainPlayer->BoundingBox.x + this->game->MainPlayer->BoundingBox.width/2,
        this->game->MainPlayer->BoundingBox.y + this->game->MainPlayer->BoundingBox.width/2}, StartPos);
    if (Distance <= 450 && Health > 0) {
        if (!BossFightStarted)
            game->MainSoundManager.PlaySoundM("badtime");
        BossFightStarted = true;
        this->game->CurrentBoss = this;
        this->game->CurrentBossName = "The Bouncing Face";
    }
    if (Health <= 0)
        BossFightStarted = false;
    if (BossFightStarted) {
        std::vector<shared_ptr<Entity>> array = game->MainEntityManager.Entities[SpawnerType];
        for (int i = 0; i < array.size(); i++) {
            if (shared_ptr<Spawner> entity = dynamic_pointer_cast<Spawner>(array.at(i)); entity != nullptr and !entity->ShouldDelete) {
                entity->SpawnerIsActive = 999;
            }
        }
    }
    Entity::Update();
    Render();
}
