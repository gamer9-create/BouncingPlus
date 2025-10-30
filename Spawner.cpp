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
    for (int i = 0; i < 8; i++) {
        RandomNumbers[i] = GetRandomValue(-100, 100) / 10.0f;
    }
}

Spawner::~Spawner() {
}

void Spawner::Update() {
    Entity::Update();
    Vector2 center_pos ={StartPos.x + BoundingBox.width / 2, StartPos.y + BoundingBox.height / 2};

    float r = 25;

    if (GetTime() - timer >= 0.1f) {
        timer = GetTime();
        RandPoint = {(float)GetRandomValue(-r, r), (float)GetRandomValue(-r, r)};
    }

    BoundingBox.x = Lerp(BoundingBox.x, StartPos.x + RandPoint.x, 5 * GetFrameTime());
    BoundingBox.y = Lerp(BoundingBox.y, StartPos.y + RandPoint.y, 5 * GetFrameTime());

    float F1 = 0;
    float F2 = 0;

    if (game->MainPlayer != nullptr && Vector2Distance(StartPos,
        {game->MainPlayer->BoundingBox.x,
        game->MainPlayer->BoundingBox.y}) < 150)
        DistF = Lerp(DistF, 35, 5*GetFrameTime());
    else
        DistF = Lerp(DistF, 0, 5*GetFrameTime());

    float RotMultiplier = 10;
    float PosMultiplier = 6;
    float Siz = 72;
    float Dec = 2;
    float TrDec = 0.8f;
    float St = 1.5f;

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
