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
    Reset();
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
            float bbox_x = curr_tile_x * TileSize;
            float bbox_y = curr_tile_y * TileSize;
            if (tile_tex != nullptr)
                DrawTexturePro(*tile_tex, Rectangle(0, 0, tile_tex->width, tile_tex->height), Rectangle(bbox_x - CameraPosition->x, bbox_y - CameraPosition->y, TileSize, TileSize), Vector2(0, 0), 0, WHITE);
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
            Weapon = "Shotgun";
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

    Reset();

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
                    EnemySpawnLocations.push_back(coord);
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

void TileManager::Reset()
{
    Map.clear();
    MapWidth = 0;
    MapHeight = 0;
    PlayerSpawnPosition = {0, 0};
    EnemySpawnLocations = std::vector<std::string>();
    TileSize = 72;
    UpdateDistance = Vector2((int) (GetScreenWidth() / 61.0f), (int)(GetScreenHeight() / 48.0f));
}

void TileManager::Quit() {

}
