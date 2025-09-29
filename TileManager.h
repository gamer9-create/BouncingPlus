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
    public:
        Game* game;
        float TileSize;
        Vector2 UpdateDistance;
        int MapWidth, MapHeight;
        std::unordered_map<std::string, int> Map;
        Texture2D BouncyWallTexture;
        Texture2D DeleteWallTexture;
        TileManager();
        TileManager(Game &game);
        void Update();
        void ReadMap(const char *Filename);
};


#endif //BOUNCINGPLUS_TILEMANAGER_H