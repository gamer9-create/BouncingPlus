//
// Created by lalit on 8/27/2025.
//

#include "TileManager.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <raymath.h>
#include <string>
#include <nlohmann/json.hpp>

#include "../entities/bosses/FaceBoss.h"
#include "../entities/subentities/Enemy.h"
#include "../game/Game.h"
#include "../entities/subentities/Spawner.h"
#include "../entities/subentities/Turret.h"
#include "../entities/subentities/UpgradeStation.h"

TileManager::TileManager() {

}

TileManager::TileManager(Game &game) {
    this->game = &game;
    BurnMarks = std::vector<BurnMark>();
    Distortions = std::vector<Distortion>();
    TileMapTex = LoadRenderTexture(1,1);
    TileTypes[0] = NothingTileType; // air
    TileTypes[1] = WallTileType; // bouncy wall
    TileTypes[2] = WallTileType; // delete wall
    TileTypes[3] = EnemyTileType; // enemy
    TileTypes[4] = EnemyTileType; // sword
    TileTypes[5] = EnemyTileType; // shotgun
    TileTypes[6] = PlayerSpawnTileType; // player spawn
    TileTypes[7] = SpawnerTileType; // enemy spawner tile type
    TileTypes[8] = EnemySpawnTileType; // enemy spawn tile type
    TileTypes[9] = UpgradeStationTileType; // upgrade station tile type
    TileTypes[10] = BossTileType; // boss
    TileTypes[11] = TurretTileType; // boss
    Clear();
}

int TileManager::GetTileAt(int x, int y)
{
    if (x < 0 || y < 0 || x >= MapWidth || y >= MapHeight)
        return -1;
    if (y * MapWidth + x >= Map.size())
    {
        cout << "map size: " << Map.size() << endl;
        return -1;
    }
    try
    {
        return Map[y * MapWidth + x];
    } catch (std::exception& e)
    {
        cout << e.what() << ", map size: " << Map.size() << endl;
        return -1;
    }
}

int TileManager::GetTileAt(Vector2 coord)
{
    return GetTileAt(coord.x,coord.y);
}

void TileManager::SetTileAt(int x, int y, int id)
{
    if (x < 0 || y < 0 || x >= MapWidth || y >= MapHeight)
        return;
    Map[y * MapWidth + x] = id;
}

void TileManager::SetTileAt(Vector2 coord, int id)
{
    SetTileAt(coord.x, coord.y, id);
}

void TileManager::Burn(Vector2 Position, Vector2 From, float Transparency)
{
    if (BurnMarks.size() >= 15)
        return;
    float BurnMarkRange = GetRandomValue(5, 8);
    Vector2 s = Vector2{(float)GetRandomValue(-BurnMarkRange, BurnMarkRange), (float)GetRandomValue(-BurnMarkRange, BurnMarkRange)};
    s = Vector2Add(s, Position);
    BurnMark burn = {
        Position, Transparency, game->GetGameTime(),
        s,
        Vector2Add(Vector2{(float)GetRandomValue(-BurnMarkRange, BurnMarkRange), (float)GetRandomValue(-BurnMarkRange, BurnMarkRange)},Position),
        Vector2Add(Vector2{(float)GetRandomValue(-BurnMarkRange, BurnMarkRange), (float)GetRandomValue(-BurnMarkRange, BurnMarkRange)},Position),
        Vector2Add(Vector2{(float)GetRandomValue(-BurnMarkRange, BurnMarkRange), (float)GetRandomValue(-BurnMarkRange, BurnMarkRange)},Position),
        s
    };
    BurnMarks.push_back(burn);
}

void TileManager::DrawTileMap()
{
    Vector2 *CameraPosition = &this->game->GameCamera.CameraPosition;
    int tile_x = static_cast<int> ((CameraPosition->x + (GetScreenWidth()/2)) / TileSize);
    int tile_y = static_cast<int> ((CameraPosition->y + (GetScreenHeight()/2)) / TileSize);
    for (int y = 0; y < UpdateDistance.y; y++) {
        for (int x = 0; x < UpdateDistance.x; x++) {
            int curr_tile_x = tile_x + x - static_cast<int> (UpdateDistance.x / 2);
            int curr_tile_y = tile_y + y - static_cast<int> (UpdateDistance.y / 2);
            int tile_id = GetTileAt(curr_tile_x,curr_tile_y);
            Texture* tile_tex = nullptr;
            if (tile_id == 1)
                tile_tex = &game->GameResources.Textures["bouncy_wall"];
            if (tile_id == 2)
                tile_tex = &game->GameResources.Textures["delete_wall"];

            if (tile_tex != nullptr) {
                float bbox_x = curr_tile_x * TileSize;
                float bbox_y = curr_tile_y * TileSize;
                bbox_x -= game->GameCamera.RaylibCamera.target.x;
                bbox_y -= game->GameCamera.RaylibCamera.target.y;
                Rectangle rec = {0,0, (float) tile_tex->width, (float) tile_tex->height};

                bool left = TileTypes[GetTileAt(curr_tile_x - 1, curr_tile_y)] == WallTileType;
                bool right = TileTypes[GetTileAt(curr_tile_x + 1, curr_tile_y)] == WallTileType;
                bool up = TileTypes[GetTileAt(curr_tile_x, curr_tile_y - 1)] == WallTileType;
                bool down = TileTypes[GetTileAt(curr_tile_x, curr_tile_y + 1)] == WallTileType;

                bool diagonal_lu = TileTypes[GetTileAt(curr_tile_x - 1, curr_tile_y - 1)] == WallTileType;
                bool diagonal_ru = TileTypes[GetTileAt(curr_tile_x + 1, curr_tile_y - 1)] == WallTileType;
                bool diagonal_ld = TileTypes[GetTileAt(curr_tile_x - 1, curr_tile_y + 1)] == WallTileType;
                bool diagonal_rd = TileTypes[GetTileAt(curr_tile_x + 1, curr_tile_y + 1)] == WallTileType;

                if (left) {
                    rec.x = 4;
                    rec.width -= 4;
                }
                if (right) {
                    rec.width -= 4;
                }
                if (up) {
                    rec.y = 4;
                    rec.height -= 4;
                }
                if (down) {
                    rec.height -= 4;
                }

                if (left) {
                    DrawTexturePro(*tile_tex, {10,0,1,36}, Rectangle(bbox_x,
                bbox_y, 8, 72), {0, 0}, 0, WHITE);
                }
                if (right) {
                    DrawTexturePro(*tile_tex, {10,0,1,36}, Rectangle(bbox_x+TileSize-8,
                bbox_y, 8, 72), {0, 0}, 0, WHITE);
                }
                if (up) {
                    DrawTexturePro(*tile_tex, {0,10,36,1}, Rectangle(bbox_x,
                bbox_y, 72, 8), {0, 0}, 0, WHITE);
                }
                if (down) {
                    DrawTexturePro(*tile_tex, {0,10,36,1}, Rectangle(bbox_x,
                bbox_y+TileSize-8, 72, 8), {0, 0}, 0, WHITE);
                }

                DrawTexturePro(*tile_tex, rec, Rectangle(bbox_x + (rec.x*2),
                    bbox_y +(rec.y*2), rec.width*2, rec.height*2), {0, 0}, 0, WHITE);

                if (left && up && diagonal_lu)
                    DrawTexturePro(*tile_tex,{10,10,1,1},
                        {bbox_x, bbox_y,8,8},
                        {0, 0},0,WHITE);
                if (left && down && diagonal_ld)
                    DrawTexturePro(*tile_tex,{10,10,1,1},
                        {bbox_x, bbox_y + TileSize-8,8,8},
                        {0, 0},0,WHITE);
                if (right && up && diagonal_ru)
                    DrawTexturePro(*tile_tex,{10,10,1,1},
                        {bbox_x+TileSize-8, bbox_y,8,8},
                        {0, 0},0,WHITE);
                if (right && down && diagonal_rd)
                    DrawTexturePro(*tile_tex,{10,10,1,1},
                        {bbox_x+TileSize-8, bbox_y + TileSize-8,8,8},
                        {0, 0},0,WHITE);
            }
        }
    }
}

void TileManager::Update() {

    if (TileMapTex.texture.width != GetScreenWidth() || TileMapTex.texture.height != GetScreenHeight()) {
        UnloadRenderTexture(TileMapTex);
        TileMapTex = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    }

    ProcessDistortions();
    ProcessBurnMarks();

    EndTextureMode();
    EndMode2D();
    BeginTextureMode(TileMapTex);
    BeginBlendMode(BLEND_ALPHA);
    ClearBackground(BLANK);

    DrawTileMap();
    BeginShaderMode(game->GameResources.Shaders["blur"]);

    int ScreenWidth = GetScreenWidth();
    int ScreenHeight = GetScreenWidth();

    SetShaderValue(game->GameResources.Shaders["blur"], game->GameCamera.uWidth2, &ScreenWidth, SHADER_UNIFORM_INT);
    SetShaderValue(game->GameResources.Shaders["blur"], game->GameCamera.uHeight2, &ScreenHeight, SHADER_UNIFORM_INT);

    for (BurnMark &b : BurnMarks)
    {
        Vector2 list[5];
        for (int i = 0; i < 5; i++)
            list[i] = Vector2Subtract(b.Points[i], game->GameCamera.RaylibCamera.target);

        float FinalTransparency = clamp(b.Transparency, 0.0f, 1.0f) * (1.0f- (game->GetGameTime() - b.SpawnTime) / (FXLifetime * 3.0f));
        DrawLineStrip(list, 5, ColorAlpha(BLACK, FinalTransparency));
        DrawCircle(b.Position.x - game->GameCamera.RaylibCamera.target.x, b.Position.y - game->GameCamera.RaylibCamera.target.y, 5, ColorAlpha(BLACK, FinalTransparency / 1.6f));
        /*
        *DrawTexturePro(game->GameResources.Textures["burn_marks"], {
            (float)(((int)b.Tex * 18) % 36), (float)(((int)b.Tex/2 * 18) % 36),
            18, 18
        }, {b.Position.x - game->GameCamera.RaylibCamera.target.x, b.Position.y - game->GameCamera.RaylibCamera.target.y, 25 * FinalTransparency, 25 * FinalTransparency},
        {25 * FinalTransparency / 2, 25 * FinalTransparency / 2}, b.Rotation, ColorAlpha(WHITE, FinalTransparency));
         */
        if (game->DebugDraw)
            DrawCircle(b.Position.x - game->GameCamera.RaylibCamera.target.x, b.Position.y - game->GameCamera.RaylibCamera.target.y, 5, ColorAlpha(PURPLE, 0.5f));
    }
    EndShaderMode();

    EndTextureMode();
    EndBlendMode();
    BeginTextureMode(game->GameCamera.CameraRenderTexture);
    BeginMode2D(game->GameCamera.RaylibCamera);

    BeginShaderMode(game->GameResources.Shaders["distortion"]);

    int DistortionCount = (int)min(100.0f, (float)Distortions.size());

    SetShaderValue(game->GameResources.Shaders["distortion"], DistortionCountLocation, &DistortionCount, SHADER_UNIFORM_INT);

    BeginBlendMode(BLEND_ALPHA);
    DrawTexturePro(TileMapTex.texture, {0, 0, (float)TileMapTex.texture.width, (float)-TileMapTex.texture.height}, {
        game->GameCamera.RaylibCamera.target.x, game->GameCamera.RaylibCamera.target.y, (float)TileMapTex.texture.width,(float)TileMapTex.texture.height
    }, {0,0}, 0, WHITE);
    EndBlendMode();
    EndShaderMode();
}

void TileManager::DistortArea(Distortion d)
{
    if (Distortions.size() >= 100)
        return;
    for (Distortion &otherDis : Distortions)
    {
        if (Vector2Distance(otherDis.Position, d.Position) < 25 && game->GetGameTime() - otherDis.SpawnTime <= FXLifetime/2)
            return;
    }
    d.SpawnTime = game->GetGameTime();
    Distortions.push_back(d);
}

void TileManager::ProcessBurnMarks()
{
    std::erase_if(BurnMarks, [this](BurnMark &d)
    {
        return game->GetGameTime() - d.SpawnTime >= FXLifetime * 3.0f;
    });
}

void TileManager::ProcessDistortions()
{
    std::erase_if(Distortions, [this](Distortion &d)
    {
        return game->GetGameTime() - d.SpawnTime >= FXLifetime;
    });
    if (DistortionUniformLocations.size() <= 0)
    {
        BeginShaderMode(game->GameResources.Shaders["distortion"]);

        for (int i = 0; i < 100; i++)
        {

            int loc1 = GetShaderLocation(game->GameResources.Shaders["distortion"], ("distortions[" + to_string(i) + "].position").c_str());
            int loc2 = GetShaderLocation(game->GameResources.Shaders["distortion"], ("distortions[" + to_string(i) + "].strength").c_str());
            int loc3 = GetShaderLocation(game->GameResources.Shaders["distortion"], ("distortions[" + to_string(i) + "].radius").c_str());
            std::tuple locs(loc1, loc2, loc3);

            DistortionUniformLocations.push_back(locs);
        }

        DistortionCountLocation = GetShaderLocation(game->GameResources.Shaders["distortion"], "distortionCount");

        EndShaderMode();
    }

    for (Distortion &d : Distortions)
    {
        if (game->GetGameTime() - d.SpawnTime < FXLifetime/2)
            d.Strength = (game->GetGameTime() - d.SpawnTime) / (FXLifetime/2);
        else if (game->GetGameTime() - d.SpawnTime >= 0.125)
            d.Strength = FXLifetime - (game->GetGameTime() - d.SpawnTime - (FXLifetime/2)) / (FXLifetime/2);

        d.Strength = clamp((float)pow(d.Strength, 2) * 3.1f, 0.0f, 3.1f);
    }

    int DistortionCount = (int)min(100.0f, (float)Distortions.size());
    for (int i = 0; i < DistortionCount; i++)
    {
        int PositionLocation = get<0>(DistortionUniformLocations[i]);
        int StrengthLocation = get<1>(DistortionUniformLocations[i]);
        int RadiusLocation = get<2>(DistortionUniformLocations[i]);

        Vector2 SPosition = GetWorldToScreen2D(Distortions[i].Position, game->GameCamera.RaylibCamera);
        if (game->DebugDraw)
            DrawCircle(Distortions[i].Position.x, Distortions[i].Position.y, 5, ColorAlpha(PINK, 0.5f));

        SetShaderValue(game->GameResources.Shaders["distortion"], PositionLocation, &SPosition, SHADER_UNIFORM_VEC2);
        SetShaderValue(game->GameResources.Shaders["distortion"], StrengthLocation, &Distortions[i].Strength, SHADER_UNIFORM_FLOAT);
        SetShaderValue(game->GameResources.Shaders["distortion"], RadiusLocation, &Distortions[i].Radius, SHADER_UNIFORM_FLOAT);
    }
}

void TileManager::AddEnemy(float bbox_x, float bbox_y, int tile_id) {
    std::string Weapon = "Default Gun";
    float Speed = 350.0f;
    float Health = 100.0f;
    float Armor = 0.0f;
    switch (tile_id)
    {
        case 4:
            Weapon = game->EnemyRoleWeapons["sword"];
            Speed = 280.0f;
            Health = 90.0f;
            Armor = 50.0f;
            break;
        case 5:
            Weapon = game->EnemyRoleWeapons["shotgun"];
            Speed = 300.0f;
            Health = 140.0f;
            Armor = 0.0f;
            break;
    }
    float Multiplier = 1.0f + 0.15f * (game->LevelData[game->CurrentLevelName]["difficulty"].get<int>()-3);
    Armor *= Multiplier;
    Speed *= Multiplier;
    Health *= Multiplier;
    game->GameEntities.AddEntity(EnemyType, make_shared<Enemy>(bbox_x, bbox_y, Health, Speed, Armor, Weapon, game->GameResources.Textures["enemy"], *game));
}

void TileManager::ReadMapDataFile(std::string Filename) {
    int y = 0;
    int x = 0;

    Clear();

    bool PlayerSpawnFound = false;

    std::ifstream  data(Filename);

    std::string line;
    while(std::getline(data,line))
    {
        std::stringstream  lineStream(line);
        std::string        cell;
        while(std::getline(lineStream,cell,','))
        {
            int tile_id = std::stoi(cell) + 1;

            Map.push_back(tile_id < 3 ? tile_id : -1);

            float bbox_x = (static_cast<float>(x) * TileSize) + TileSize / 2.0f;
            float bbox_y = (static_cast<float>(y) * TileSize) + TileSize / 2.0f;

            switch (TileTypes[tile_id]) {
                case EnemyTileType:
                    AddEnemy(bbox_x, bbox_y, tile_id);
                    break;
                case EnemySpawnTileType:
                    EnemySpawnLocations.push_back({bbox_x,bbox_y});
                    break;
                case PlayerSpawnTileType:
                    PlayerSpawnFound = true;
                    PlayerSpawnPosition = Vector2(bbox_x, bbox_y);
                    break;
                case SpawnerTileType: {
                    std::shared_ptr<Spawner> spawner = std::make_shared<Spawner>(*game, bbox_x, bbox_y);
                    game->GameEntities.AddEntity(SpawnerType, spawner);
                    break;
                }
                case BossTileType: {
                    BossSpawnPosition = Vector2(bbox_x, bbox_y);
                    break;
                };
                case UpgradeStationTileType: {
                    std::shared_ptr<UpgradeStation> station = std::make_shared<UpgradeStation>(*game, bbox_x, bbox_y);
                    game->GameEntities.AddEntity(UpgradeStationType, station);
                    break;
                }
                case TurretTileType: {
                    std::shared_ptr<Turret> t = std::make_shared<Turret>(*game,
                        "Sniper", bbox_x, bbox_y);
                    game->GameEntities.AddEntity(TurretType, t);
                    break;
                }
            }

            x += 1;
        }
        if (x > MapWidth) {
            MapWidth = x;
        }
        x = 0;
        y += 1;
    }
    MapHeight = y;

    if (!PlayerSpawnFound)
        PlayerSpawnPosition = {MapWidth * TileSize / 2.0f, MapHeight * TileSize / 2.0f};
}

void TileManager::Clear()
{
    Map.clear();
    Distortions.clear();
    BurnMarks.clear();
    MapWidth = 0;
    MapHeight = 0;
    PlayerSpawnPosition = {0, 0};
    BossSpawnPosition = {0,0};
    EnemySpawnLocations = std::vector<Vector2>();
    FXLifetime = 0.75f;
    TileSize = 72;
    UpdateDistance = Vector2((int) (GetScreenWidth() / 61.0f), (int)(GetScreenHeight() / 48.0f));;
    if (IsRenderTextureValid(TileMapTex))
        UnloadRenderTexture(TileMapTex);
    TileMapTex = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
}

void TileManager::Quit() {
    Clear();
    UnloadRenderTexture(TileMapTex);
}
