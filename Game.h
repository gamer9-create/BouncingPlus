//
// Created by lalit on 8/26/2025.
//

#ifndef BOUNCINGPLUS_GAME_H
#define BOUNCINGPLUS_GAME_H

#include <raylib.h>
#include <vector>
#include "CameraManager.h"
#include "Entity.h"
#include "EntityManager.h"
#include "ParticleManager.h"
#include "Player.h"
#include "TileManager.h"
#include "UIManager.h"
#include "Weapons.h"

using namespace std;

class Game {

    float SlowdownTime;
    float MaxSlowdownTime;
    float SlowdownShakeIntensity;
    std::string current_map_filename;
    UIManager MainUIManager;
    void SetGameData();
    void ProcessSlowdownAnimation();
    public:
        bool Paused;
        float GameSpeed;
        TileManager MainTileManager;
        EntityManager MainEntityManager;
        CameraManager MainCameraManager;
        unordered_map<std::string, Texture2D> Textures;
        unordered_map<std::string, Sound> Sounds;
        unordered_map<std::string, Weapon> Weapons;
        ParticleManager MainParticleManager;
        shared_ptr<Player> MainPlayer;
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