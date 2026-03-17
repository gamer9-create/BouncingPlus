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

struct Distortion
{
    Vector2 Position;
    float Strength;
    float Radius;
    double SpawnTime;
};

class TileManager {
    Game* game;
    Vector2 UpdateDistance;

    std::vector<std::tuple<int, int, int>> DistortionUniformLocations;
    std::vector<Distortion> Distortions;

    int DistortionCountLocation;
    float DistortionLifetime;

    void DrawTileMap();
    void ProcessDistortions();
    void AddEnemy(float bbox_x, float bbox_y, int tile_id);

    public:
        float TileSize;
        int MapWidth, MapHeight;
        Vector2 PlayerSpawnPosition;
        Vector2 BossSpawnPosition;
        TileType TileTypes[11];
        std::vector<Vector2> EnemySpawnLocations;
        std::unordered_map<std::string, int> Map;
        RenderTexture TileMapTex;
        TileManager();
        TileManager(Game &game);
        void Update();
        void DistortArea(Distortion d);
        void ReadMapDataFile(std::string Filename);
        void Clear();
        void Quit();
};


#endif //BOUNCINGPLUS_TILEMANAGER_H