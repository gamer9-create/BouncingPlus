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

TileManager::TileManager() {

}

TileManager::TileManager(Game &game) {
    this->game = &game;
    Reset();
    BouncyWallTexture = LoadTexture("assets/img/bouncy_wall.png");
    DeleteWallTexture = LoadTexture("assets/img/delete_wall.png");
}

void TileManager::Update() {
    Vector2 *CameraPosition = &this->game->MainCamera.CameraPosition;
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
                tile_tex = &BouncyWallTexture;
            if (tile_id == 2)
                tile_tex = &DeleteWallTexture;
            float bbox_x = curr_tile_x * TileSize;
            float bbox_y = curr_tile_y * TileSize;
            if (tile_tex != nullptr)
                DrawTexturePro(*tile_tex, Rectangle(0, 0, tile_tex->width, tile_tex->height), Rectangle(bbox_x - CameraPosition->x, bbox_y - CameraPosition->y, TileSize, TileSize), Vector2(0, 0), 0, WHITE);
        }
    }
}


void TileManager::ReadMapDataFile(std::string Filename) {
    int y = 0;
    int x = 0;

    MapWidth = 0;
    MapHeight = 0;
    Map.clear();

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

            if (tile_id < 3) {
                Map.insert({coord, tile_id});
            } else {
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
            x += 1;
        }
        if (x > MapWidth) {
            MapWidth = x;
        }
        x = 0;
        y += 1;
    }
    MapHeight = y;
}

void TileManager::Reset()
{
    Map.clear();
    MapWidth = 0;
    MapHeight = 0;
    TileSize = 72;
    UpdateDistance = Vector2((int) (GetScreenWidth() / 61.0f), (int)(GetScreenHeight() / 48.0f));
    MapWidth = 0;
    MapHeight = 0;
}

void TileManager::Quit() {
    UnloadTexture(BouncyWallTexture);
    UnloadTexture(DeleteWallTexture);
}
