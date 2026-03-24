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

struct BurnMark
{
    Vector2 Position;
    float Transparency;
    double SpawnTime;
    Vector2 Points[5];
};

class TileManager {
    Game* game;
    Vector2 UpdateDistance;

    std::vector<std::tuple<int, int, int>> DistortionUniformLocations;
    std::vector<Distortion> Distortions;
    std::vector<BurnMark> BurnMarks;

    int DistortionCountLocation;
    float FXLifetime;

    void DrawTileMap();
    void ProcessDistortions();
    void ProcessBurnMarks();
    void AddEnemy(float bbox_x, float bbox_y, int tile_id);

    public:
        float TileSize;
        int MapWidth, MapHeight;
        Vector2 PlayerSpawnPosition;
        Vector2 BossSpawnPosition;
        TileType TileTypes[12];
        std::vector<Vector2> EnemySpawnLocations;
        std::vector<int> Map;
        RenderTexture TileMapTex;
        TileManager();
        TileManager(Game &game);
        int GetTileAt(int x, int y);
        int GetTileAt(Vector2 coord);
        void SetTileAt(int x, int y, int id);
        void SetTileAt(Vector2 coord, int id);
        void Update();
        void DistortArea(Distortion d);
        void Burn(Vector2 Position, Vector2 From, float Transparency);
        void ReadMapDataFile(std::string Filename);
        void Clear();
        void Quit();
};


#endif //BOUNCINGPLUS_TILEMANAGER_H