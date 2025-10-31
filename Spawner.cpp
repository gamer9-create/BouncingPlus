//
// Created by lalit on 10/28/2025.
//

#include "Spawner.h"

#include <raymath.h>

#include "Game.h"

Spawner::Spawner() {
}

Spawner::Spawner(Game &game, float bbox_x, float bbox_y) :Entity(game.Textures["spawner"], {bbox_x, bbox_y, 36, 36}, 0, game) {
    StartPos = Vector2(bbox_x, bbox_y);
    RandPoint = {0, 0};
    DistF =0;
    SpawnerIsActive = 0;
    SpawnerRageCooldown = 0;
    PosMultiplier = 10;
    RotMultiplier = 6;
    SpawnTimer = GetTime();
    for (int i = 0; i < 8; i++) {
        RandomNumbers[i] = GetRandomValue(-100, 100) / 10.0f;
    }
}

void Spawner::Render() {
    Vector2 center_pos ={StartPos.x + BoundingBox.width / 2, StartPos.y + BoundingBox.height / 2};

    float r = 25;

    if (GetTime() - timer >= 0.1f) {
        timer = GetTime();
        RandPoint = {(float)GetRandomValue(-r, r), (float)GetRandomValue(-r, r)};
    }

    BoundingBox.x = Lerp(BoundingBox.x, StartPos.x + RandPoint.x, ((SpawnerIsActive>0) ? 15 : 5) * GetFrameTime());
    BoundingBox.y = Lerp(BoundingBox.y, StartPos.y + RandPoint.y, ((SpawnerIsActive>0) ? 15 : 5) * GetFrameTime());

    float F1 = 0;
    float F2 = 0;

    if (SpawnerIsActive <= 0) {
        if (game->MainPlayer != nullptr && Vector2Distance(StartPos,
            {game->MainPlayer->BoundingBox.x,
            game->MainPlayer->BoundingBox.y}) < 150) {
            DistF = Lerp(DistF, 35, 5*GetFrameTime());
            this->game->MainCameraManager.QuickZoom(1.5f, 0.1f);
        }else
            DistF = Lerp(DistF, 0, 5*GetFrameTime());
        this->EntityColor = ColorLerp(EntityColor, WHITE, 3 * GetFrameTime());
    } else {
        DistF = Lerp(DistF, 50, 15*GetFrameTime());
        this->EntityColor = ColorLerp(EntityColor, RED, 1.5f * GetFrameTime());
    }

    float Siz = 72;
    float Dec = 2;
    float TrDec = 0.8f;
    float St = 1.5f;

    if (SpawnerIsActive>0) {
        RotMultiplier = Lerp(RotMultiplier, 60, 5 * GetFrameTime());
        PosMultiplier = Lerp(PosMultiplier, 100, 5 * GetFrameTime());
    } else {
        RotMultiplier = Lerp(RotMultiplier, 6, 5 * GetFrameTime());
        PosMultiplier = Lerp(PosMultiplier, 10, 5 * GetFrameTime());
    }

    Color F3 = {0, 255, 0, 255};

    F1 = sin(GetTime()+RandomNumbers[0]);
    F2 = cos(GetTime()+RandomNumbers[1]);
    F1 /= Dec;
    F2 /= Dec;
    DrawTexturePro(game->Textures["bouncy_wall"], {0, 0, 18, 18},
        {center_pos.x - game->MainCameraManager.CameraPosition.x + F1 * PosMultiplier - DistF,
            center_pos.y - game->MainCameraManager.CameraPosition.y + F2 * PosMultiplier - DistF,
            Siz, Siz},
        {Siz, Siz}, F1 * RotMultiplier, ColorAlpha(F3, St + F1/TrDec));
    F1 = sin(GetTime()+RandomNumbers[2]);
    F2 = cos(GetTime()+RandomNumbers[3]);
    F1 /= Dec;
    F2 /= Dec;
    DrawTexturePro(game->Textures["bouncy_wall"], {18, 0, 18, 18},
        {center_pos.x - game->MainCameraManager.CameraPosition.x + F1 * PosMultiplier + DistF,
            center_pos.y - game->MainCameraManager.CameraPosition.y + F2 * PosMultiplier-DistF,
            Siz, Siz},
        {0, Siz}, F1 * RotMultiplier, ColorAlpha(F3, St + F1/TrDec));
    F1 = sin(GetTime()+RandomNumbers[4]);
    F2 = cos(GetTime()+RandomNumbers[5]);
    F1 /= Dec;
    F2 /= Dec;
    DrawTexturePro(game->Textures["bouncy_wall"], {0, 18, 18, 18},
        {center_pos.x - game->MainCameraManager.CameraPosition.x + F1 * PosMultiplier - DistF,
            center_pos.y - game->MainCameraManager.CameraPosition.y + F2 * PosMultiplier + DistF,
            Siz, Siz},
        {Siz, 0}, F1 * RotMultiplier, ColorAlpha(F3, St + F1/TrDec));
    F1 = sin(GetTime()+RandomNumbers[6]);
    F2 = cos(GetTime()+RandomNumbers[7]);
    F1 /= Dec;
    F2 /= Dec;
    DrawTexturePro(game->Textures["bouncy_wall"], {18, 18, 18, 18},
        {center_pos.x - game->MainCameraManager.CameraPosition.x + F1 * PosMultiplier + DistF,
            center_pos.y - game->MainCameraManager.CameraPosition.y + F2 * PosMultiplier+ DistF,
            Siz, Siz},
        {0, 0}, F1 * RotMultiplier, ColorAlpha(F3, St + F1/TrDec));
}

Spawner::~Spawner() {
}

void Spawner::Update() {
    if (SpawnerIsActive > 0) {
        SpawnerIsActive -= GetFrameTime();
    } else {
        if (SpawnerRageCooldown > 0)
            SpawnerRageCooldown -= GetFrameTime();
        EntitiesSpawned = 0;
    }
    Vector2 PlrPos = {game->MainPlayer->BoundingBox.x,
        game->MainPlayer->BoundingBox.y};
    if (game->MainPlayer != nullptr && Vector2Distance(StartPos,
        PlrPos) < 50 && SpawnerIsActive <= 0 && SpawnerRageCooldown <= 0) {
        game->MainPlayer->VelocityMovement = Vector2Subtract(StartPos, PlrPos);
        game->MainPlayer->VelocityPower = -1500;
        PlaySound(game->Sounds["spawner_activate"]);
        PlaySound(game->Sounds["spawner_boom"]);
        game->MainCameraManager.ShakeCamera(0.5f);
        SpawnerIsActive = 60;
        SpawnerRageCooldown = 15;
        SpawnTimer = GetTime();
    }

    if (SpawnerIsActive > 0 && EntitiesSpawned < 10 && GetTime() - SpawnTimer >= 3 &&
        game->MainTileManager.EnemySpawnLocations.size() > 0) {
        Vector2 p = game->MainTileManager.EnemySpawnLocations[GetRandomValue(0,
            game->MainTileManager.EnemySpawnLocations.size() - 1)];
        std::shared_ptr<Enemy> e = make_shared<Enemy>(p.x, p.y, 100, 150, 0, "Default Gun", game->Textures["spawned_enemy"], *game);
        game->MainEntityManager.AddEntity(EnemyType, e);
        EntitiesSpawned++;
        SpawnTimer = GetTime();
    }

    Entity::Update();
    Render();
}
