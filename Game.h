//
// Created by lalit on 8/26/2025.
//

#ifndef BOUNCINGPLUS_GAME_H
#define BOUNCINGPLUS_GAME_H

#include <memory>
#include <raylib.h>
#include <vector>
#include "Bullet.h"
#include "Entity.h"
#include "Player.h"
#include "TileManager.h"
#include "UI.h"
#include "Weapons.h"

using namespace std;

class Game {
    public:
        Vector2 CameraPosition;
        Vector2 CameraTarget;
        float CameraSpeed;
        float PhysicsFPS;
        float PhysicsAccumulator;
        UI Ui;
        TileManager MainTileManager;
        unordered_map<std::string, Texture2D> Textures;
        unordered_map<std::string, Weapon> Weapons;
        shared_ptr<Player> MainPlayer;
        vector<shared_ptr<Entity>> Entities;
        vector<shared_ptr<Bullet>> Bullets;
        Game();
        void SetGameData();
        void Reload(const char *Filename);
        void Update();
        void Quit();
};


#endif //BOUNCINGPLUS_GAME_H