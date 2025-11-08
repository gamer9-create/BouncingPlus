//
// Created by lalit on 8/27/2025.
//

#include "TileManager.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include "Enemy.h"
#include "Game.h"
#include "Spawner.h"

TileManager::TileManager() {

}

TileManager::TileManager(Game &game) {
    this->game = &game;
    Clear();
    TileTypes[0] = NothingTileType; // air
    TileTypes[1] = WallTileType; // bouncy wall
    TileTypes[2] = WallTileType; // delete wall
    TileTypes[3] = EnemyTileType; // enemy
    TileTypes[4] = EnemyTileType; // sword
    TileTypes[5] = EnemyTileType; // shotgun
    TileTypes[6] = PlayerSpawnTileType; // player spawn
    TileTypes[7] = SpawnerTileType; // enemy spawner tile type
    TileTypes[8] = EnemySpawnTileType; // enemy spawn tile type

}

void TileManager::Update() {
    Vector2 *CameraPosition = &this->game->MainCameraManager.CameraPosition;
    int tile_x = static_cast<int> ((CameraPosition->x + (GetScreenWidth()/2)) / TileSize);
    int tile_y = static_cast<int> ((CameraPosition->y + (GetScreenHeight()/2)) / TileSize);
    for (int y = 0; y < UpdateDistance.y; y++) {
        for (int x = 0; x < UpdateDistance.x; x++) {
            int curr_tile_x = tile_x + x - static_cast<int> (UpdateDistance.x / 2);
            int curr_tile_y = tile_y + y - static_cast<int> (UpdateDistance.y / 2);
            std::string coord = std::to_string(curr_tile_x) + " " + std::to_string(curr_tile_y);
            int tile_id = Map[coord];
            Texture* tile_tex = nullptr;
            if (tile_id == 1)
                tile_tex = &game->Textures["bouncy_wall"];
            if (tile_id == 2)
                tile_tex = &game->Textures["delete_wall"];

            if (tile_tex != nullptr) {
                float bbox_x = curr_tile_x * TileSize;
                float bbox_y = curr_tile_y * TileSize;
                Rectangle rec = {0,0, (float) tile_tex->width, (float) tile_tex->height};

                bool left = TileTypes[Map[std::to_string(curr_tile_x-1) + " " + std::to_string(curr_tile_y)]] == WallTileType;
                bool right = TileTypes[Map[std::to_string(curr_tile_x+1) + " " + std::to_string(curr_tile_y)]] == WallTileType;
                bool up = TileTypes[Map[std::to_string(curr_tile_x) + " " + std::to_string(curr_tile_y-1)]] == WallTileType;
                bool down = TileTypes[Map[std::to_string(curr_tile_x) + " " + std::to_string(curr_tile_y+1)]] == WallTileType;

                bool diagonal_lu = TileTypes[Map[std::to_string(curr_tile_x-1) + " " + std::to_string(curr_tile_y-1)]] == WallTileType;
                bool diagonal_ru = TileTypes[Map[std::to_string(curr_tile_x+1) + " " + std::to_string(curr_tile_y-1)]] == WallTileType;
                bool diagonal_ld = TileTypes[Map[std::to_string(curr_tile_x-1) + " " + std::to_string(curr_tile_y+1)]] == WallTileType;
                bool diagonal_rd = TileTypes[Map[std::to_string(curr_tile_x+1) + " " + std::to_string(curr_tile_y+1)]] == WallTileType;

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
                    DrawTexturePro(*tile_tex, {10,0,1,36}, Rectangle(bbox_x - CameraPosition->x,
                bbox_y - CameraPosition->y, 8, 72), {}, 0, WHITE);
                }
                if (right) {
                    DrawTexturePro(*tile_tex, {10,0,1,36}, Rectangle(bbox_x - CameraPosition->x+TileSize-8,
                bbox_y - CameraPosition->y, 8, 72), {}, 0, WHITE);
                }
                if (up) {
                    DrawTexturePro(*tile_tex, {0,10,36,1}, Rectangle(bbox_x - CameraPosition->x,
                bbox_y - CameraPosition->y, 72, 8), {}, 0, WHITE);
                }
                if (down) {
                    DrawTexturePro(*tile_tex, {0,10,36,1}, Rectangle(bbox_x - CameraPosition->x,
                bbox_y - CameraPosition->y+TileSize-8, 72, 8), {}, 0, WHITE);
                }

                DrawTexturePro(*tile_tex, rec, Rectangle(bbox_x - CameraPosition->x + (rec.x*2),
                    bbox_y - CameraPosition->y +(rec.y*2), rec.width*2, rec.height*2), {}, 0, WHITE);

                if (left && up && diagonal_lu)
                    DrawTexturePro(*tile_tex,{10,10,1,1},
                        {bbox_x - CameraPosition->x, bbox_y - CameraPosition->y,8,8},
                        {},0,WHITE);
                if (left && down && diagonal_ld)
                    DrawTexturePro(*tile_tex,{10,10,1,1},
                        {bbox_x - CameraPosition->x, bbox_y - CameraPosition->y + TileSize-8,8,8},
                        {},0,WHITE);
                if (right && up && diagonal_ru)
                    DrawTexturePro(*tile_tex,{10,10,1,1},
                        {bbox_x - CameraPosition->x+TileSize-8, bbox_y - CameraPosition->y,8,8},
                        {},0,WHITE);
                if (right && down && diagonal_rd)
                    DrawTexturePro(*tile_tex,{10,10,1,1},
                        {bbox_x - CameraPosition->x+TileSize-8, bbox_y - CameraPosition->y + TileSize-8,8,8},
                        {},0,WHITE);
            }
        }
    }
}

void TileManager::AddEnemy(float bbox_x, float bbox_y, int tile_id) {
    std::string Weapon = "Default Gun";
    float Speed = 250.0f;
    float Health = 100.0f;
    float Armor = 0.0f;
    switch (tile_id)
    {
        case 4:
            Weapon = "Enemy Sword";
            Speed = 125.0f;
            Health = 90.0f;
            Armor = 50.0f;
            break;
        case 5:
            Weapon = "Enemy Shotgun";
            Speed = 250.0f;
            Health = 140.0f;
            Armor = 0.0f;
            break;
    }
    game->MainEntityManager.AddEntity(EnemyType, make_shared<Enemy>(bbox_x, bbox_y, Health, Speed, Armor, Weapon, game->Textures["enemy"], *game));
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
            std::string coord = std::to_string(x) + " " + std::to_string(y);
            int tile_id = std::stoi(cell)+1;

            float bbox_x = (static_cast<float>(x) * TileSize) + TileSize / 2.0f;
            float bbox_y = (static_cast<float>(y) * TileSize) + TileSize / 2.0f;

            switch (TileTypes[tile_id]) {
                case WallTileType:
                    Map.insert({coord, tile_id});
                    break;
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
                case SpawnerTileType:
                    std::shared_ptr<Spawner> spawner = std::make_shared<Spawner>(*game, bbox_x, bbox_y);
                    game->MainEntityManager.AddEntity(SpawnerType, spawner);
                    break;

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
    MapWidth = 0;
    MapHeight = 0;
    PlayerSpawnPosition = {0, 0};
    EnemySpawnLocations = std::vector<Vector2>();
    TileSize = 72;
    UpdateDistance = Vector2((int) (GetScreenWidth() / 61.0f), (int)(GetScreenHeight() / 48.0f));
}

void TileManager::Quit() {

}
