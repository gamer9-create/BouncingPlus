//
// Created by lalit on 8/27/2025.
//

#ifndef BOUNCINGPLUS_TILEMANAGER_H
#define BOUNCINGPLUS_TILEMANAGER_H

#include <raylib.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "TileType.h"

class Game;

class TileManager {
    Game* game;
    Vector2 UpdateDistance;

    void AddEnemy(float bbox_x, float bbox_y, int tile_id);

    public:
        float TileSize;
        int MapWidth, MapHeight;
        Vector2 PlayerSpawnPosition;
        TileType TileTypes[9];
        std::vector<std::string> EnemySpawnLocations;
        std::unordered_map<std::string, int> Map;
        TileManager();
        TileManager(Game &game);
        void Update();
        void ReadMapDataFile(std::string Filename);
        void Reset();
        void Quit();
};


#endif //BOUNCINGPLUS_TILEMANAGER_H