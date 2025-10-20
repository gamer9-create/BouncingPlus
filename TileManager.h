//
// Created by lalit on 8/27/2025.
//

#ifndef BOUNCINGPLUS_TILEMANAGER_H
#define BOUNCINGPLUS_TILEMANAGER_H

#include <raylib.h>
#include <string>
#include <unordered_map>

class Game;

class TileManager {
    Texture2D BouncyWallTexture;
    Texture2D DeleteWallTexture;
    Game* game;
    Vector2 UpdateDistance;
    public:
        float TileSize;
        int MapWidth, MapHeight;
        std::unordered_map<std::string, int> Map;

        TileManager();
        TileManager(Game &game);
        void Update();
        void ReadMapDataFile(std::string Filename);
        void Quit();
};


#endif //BOUNCINGPLUS_TILEMANAGER_H