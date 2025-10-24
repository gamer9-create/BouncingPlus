//
// Created by lalit on 8/26/2025.
//

#ifndef BOUNCINGPLUS_GAME_H
#define BOUNCINGPLUS_GAME_H

#include <raylib.h>
#include <vector>
#include "Bullet.h"
#include "Camera.h"
#include "Entity.h"
#include "ParticleSystem.h"
#include "Player.h"
#include "TileManager.h"
#include "UI.h"
#include "Weapons.h"

using namespace std;

class Game {

    float GameSpeed;
    float PhysicsFPS;
    float PhysicsAccumulator;
    float SlowdownTime;
    float MaxSlowdownTime;
    float SlowdownShakeIntensity;
    std::string current_map_filename;
    UI Ui;
    void SetGameData();
    void EntityUpdate();
    void EntityPhysicsUpdate();
    void ProcessSlowdownAnimation();
    public:
        bool Paused;
        TileManager MainTileManager;
        GameCamera MainCamera;
        unordered_map<std::string, Texture2D> Textures;
        unordered_map<std::string, Sound> Sounds;
        unordered_map<std::string, Weapon> Weapons;
        ParticleSystem MainParticleSystem;
        shared_ptr<Player> MainPlayer;
        std::unordered_map<EntityType, std::vector<shared_ptr<Entity>>> Entities;
        bool DebugDraw;
        Game();
        bool RayCastSpecifiedPrecision(Vector2 origin, Vector2 target, float Precision = 36);
        bool RayCast(Vector2 origin, Vector2 target);
        void Slowdown(float Time);
        void Slowdown(float Time, float ShakeIntensity);
        void Reload(std::string Filename);
        void Update(Camera2D camera);
        void Clear();
        void Quit();
};


#endif //BOUNCINGPLUS_GAME_H