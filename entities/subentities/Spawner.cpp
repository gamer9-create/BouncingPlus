//
// Created by lalit on 10/28/2025.
//

#include "Spawner.h"

#include <raymath.h>

#include "../../game/Game.h"

Spawner::Spawner() {
}

Spawner::Spawner(Game &game, float bbox_x, float bbox_y) :Entity(game.Textures["spawner"], {bbox_x, bbox_y, 36, 36}, 0, game) {
    StartPos = Vector2(bbox_x, bbox_y);
    RandPoint = {0, 0};
    DistF =0;
    this->Type = SpawnerType;
    SpawnerIsActive = 0;
    SpawnerRageCooldown = 0;
    PosMultiplier = 10;
    this->EnemyDifficulty = 1.0f;
    this->SpawnCooldown = 3.0f;
    RotMultiplier = 6;
    SpawnTimer = game.GetGameTime();
    for (int i = 0; i < 8; i++) {
        RandomNumbers[i] = GetRandomValue(-100, 100) / 10.0f;
    }
}

void Spawner::Render() {
    Vector2 center_pos ={StartPos.x + BoundingBox.width / 2, StartPos.y + BoundingBox.height / 2};

    float r = 25;

    if (game->GetGameTime() - timer >= 0.1f) {
        timer = game->GetGameTime();
        RandPoint = {(float)GetRandomValue(-r, r), (float)GetRandomValue(-r, r)};
    }

    BoundingBox.x = Lerp(BoundingBox.x, StartPos.x + RandPoint.x, ((SpawnerIsActive>0) ? 15 : 5) * game->GetGameDeltaTime());
    BoundingBox.y = Lerp(BoundingBox.y, StartPos.y + RandPoint.y, ((SpawnerIsActive>0) ? 15 : 5) * game->GetGameDeltaTime());

    float F1 = 0;
    float F2 = 0;

    if (SpawnerIsActive <= 0) {
        if (game->MainPlayer != nullptr && Vector2Distance(StartPos,
            {game->MainPlayer->BoundingBox.x,
            game->MainPlayer->BoundingBox.y}) < 150) {
            DistF = Lerp(DistF, 35, 5*game->GetGameDeltaTime());
            this->game->MainCameraManager.QuickZoom(1.5f, 0.1f);
        }else
            DistF = Lerp(DistF, 0, 5*game->GetGameDeltaTime());
        this->EntityColor = ColorLerp(EntityColor, WHITE, 3 * game->GetGameDeltaTime());
    } else {
        DistF = Lerp(DistF, 5, 15*game->GetGameDeltaTime());
        this->EntityColor = ColorLerp(EntityColor, RED, 1.5f * game->GetGameDeltaTime());
    }

    float Siz = 72;
    float Dec = 2;
    float TrDec = 0.6f;
    float St = 1.5f;

    if (SpawnerIsActive>0) {
        RotMultiplier = Lerp(RotMultiplier, 15, 2.5f * game->GetGameDeltaTime());
        PosMultiplier = Lerp(PosMultiplier, 48, 2.5f * game->GetGameDeltaTime());
        TrDec = 1;
    } else {
        RotMultiplier = Lerp(RotMultiplier, 6, 2.5f * game->GetGameDeltaTime());
        PosMultiplier = Lerp(PosMultiplier, 10, 2.5f * game->GetGameDeltaTime());
    }

    Color F3 = {0, 255, 0, 255};

    F1 = sin(game->GetGameTime()+RandomNumbers[0]);
    F2 = cos(game->GetGameTime()+RandomNumbers[1]);
    F1 /= Dec;
    F2 /= Dec;
    DrawTexturePro(game->Textures["bouncy_wall"], {0, 0, 18, 18},
        {center_pos.x + F1 * PosMultiplier - DistF,
            center_pos.y + F2 * PosMultiplier - DistF,
            Siz, Siz},
        {Siz, Siz}, F1 * RotMultiplier, ColorAlpha(F3, St + F1/TrDec));
    F1 = sin(game->GetGameTime()+RandomNumbers[2]);
    F2 = cos(game->GetGameTime()+RandomNumbers[3]);
    F1 /= Dec;
    F2 /= Dec;
    DrawTexturePro(game->Textures["bouncy_wall"], {18, 0, 18, 18},
        {center_pos.x + F1 * PosMultiplier + DistF,
            center_pos.y + F2 * PosMultiplier-DistF,
            Siz, Siz},
        {0, Siz}, F1 * RotMultiplier, ColorAlpha(F3, St + F1/TrDec));
    F1 = sin(game->GetGameTime()+RandomNumbers[4]);
    F2 = cos(game->GetGameTime()+RandomNumbers[5]);
    F1 /= Dec;
    F2 /= Dec;
    DrawTexturePro(game->Textures["bouncy_wall"], {0, 18, 18, 18},
        {center_pos.x + F1 * PosMultiplier - DistF,
            center_pos.y + F2 * PosMultiplier + DistF,
            Siz, Siz},
        {Siz, 0}, F1 * RotMultiplier, ColorAlpha(F3, St + F1/TrDec));
    F1 = sin(game->GetGameTime()+RandomNumbers[6]);
    F2 = cos(game->GetGameTime()+RandomNumbers[7]);
    F1 /= Dec;
    F2 /= Dec;
    DrawTexturePro(game->Textures["bouncy_wall"], {18, 18, 18, 18},
        {center_pos.x + F1 * PosMultiplier + DistF,
            center_pos.y + F2 * PosMultiplier+ DistF,
            Siz, Siz},
        {0, 0}, F1 * RotMultiplier, ColorAlpha(F3, St + F1/TrDec));
}

Spawner::~Spawner() {
}

void Spawner::Update() {
    if (SpawnerIsActive > 0) {
        SpawnerIsActive -= game->GetGameDeltaTime();
    } else {
        if (SpawnerRageCooldown > 0)
            SpawnerRageCooldown -= game->GetGameDeltaTime();
        EntitiesSpawned = 0;
    }
    Vector2 PlrPos = {game->MainPlayer->BoundingBox.x,
        game->MainPlayer->BoundingBox.y};
    if (game->MainPlayer != nullptr && Vector2Distance(StartPos,
        PlrPos) < 50 && SpawnerIsActive <= 0 && SpawnerRageCooldown <= 0) {
        game->MainPlayer->VelocityMovement = Vector2Subtract(StartPos, PlrPos);
        game->MainPlayer->VelocityPower = -1500;
        game->MainSoundManager.PlaySoundM("spawner_activate");
        game->MainSoundManager.PlaySoundM("spawner_boom");
        game->MainCameraManager.ShakeCamera(0.5f);
        SpawnerIsActive = GetRandomValue(30, 90);
        SpawnerRageCooldown = GetRandomValue(5, 10);
        SpawnTimer = game->GetGameTime();
    }

    if (SpawnerIsActive > 0 && game->GetGameTime() - SpawnTimer >= SpawnCooldown && game->MainTileManager.EnemySpawnLocations.size() > 0){
        for (int i = 0; i < GetRandomValue(1, 4); i++)
        {
            Vector2 p = game->MainTileManager.EnemySpawnLocations[GetRandomValue(0,
                game->MainTileManager.EnemySpawnLocations.size() - 1)];
            Vector2 g = Vector2Add(p, {(float)GetRandomValue(-100, 100), (float)GetRandomValue(-100, 100)});
            while (true)
            {
                int tile_x = static_cast<int> (g.x / game->MainTileManager.TileSize);
                int tile_y = static_cast<int> (g.y / game->MainTileManager.TileSize);
                std::string coord = std::to_string(tile_x) + " " + std::to_string(tile_y);
                int tile_id = game->MainTileManager.Map[coord];
                if (game->MainTileManager.TileTypes[tile_id] == WallTileType)
                {
                    g = Vector2Add(p, {(float)GetRandomValue(-100, 100), (float)GetRandomValue(-100, 100)});
                } else
                {
                    break;
                }
            }

            std::shared_ptr<Enemy> e = make_shared<Enemy>(p.x, p.y, 75, 150 + (GetRandomValue(1, 100) / 10.0f < EnemyDifficulty ? GetRandomValue(10, 75) : 0), GetRandomValue(1, 100) / 100.0f < EnemyDifficulty ? GetRandomValue(50, 100) : 0, game->EnemyWeaponNamesList[GetRandomValue(0, game->EnemyWeaponNamesList.size() - 1)], game->Textures["spawned_enemy"], *game);
            e->WanderingEnabled = true;            game->MainEntityManager.AddEntity(EnemyType, e);
            game->MainParticleManager.ParticleEffect({
                p,
                150,
                WHITE,
                40,
                4,
                2,
                GRAY
            }, 0, 360, 55);
            EntitiesSpawned++;
            SpawnTimer = game->GetGameTime();
        }
    }

    Entity::Update();
    Render();
}
