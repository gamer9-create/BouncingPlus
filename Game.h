//
// Created by lalit on 8/26/2025.
//

#ifndef BOUNCINGPLUS_GAME_H
#define BOUNCINGPLUS_GAME_H

#include <raylib.h>
#include <nlohmann/json_fwd.hpp>
#include "CameraManager.h"
#include "Entity.h"
#include "EntityManager.h"
#include "ParticleManager.h"
#include "Player.h"
#include "SoundManager.h"
#include "TileManager.h"
#include "UIManager.h"
#include "Weapons.h"

using namespace std;

class Game {

    float SlowdownTime;
    float MaxSlowdownTime;
    float SlowdownShakeIntensity;

    UIManager MainUIManager;
    void SetGameData();
    void ProcessSlowdownAnimation();
    void DisplayPickups();

    public:
        bool Paused;
        float GameSpeed;
        std::string CurrentLevelName;
        std::unordered_map<std::string, nlohmann::json> LevelData;
        TileManager MainTileManager;
        EntityManager MainEntityManager;
        CameraManager MainCameraManager;
        ParticleManager MainParticleManager;
        SoundManager MainSoundManager;
        shared_ptr<Player> MainPlayer;
        std::vector<WeaponPickup> WeaponPickups;
        unordered_map<std::string, Texture2D> Textures;
        unordered_map<std::string, Shader> Shaders;
        std::vector<std::string> WeaponNamesList;
        unordered_map<std::string, Weapon> Weapons;
        bool DebugDraw;
        bool ShouldReturn;
        Game(std::unordered_map<std::string, nlohmann::json> json);
        bool RayCastSP(Vector2 origin, Vector2 target, float Precision = 36);
        bool RayCast(Vector2 origin, Vector2 target);
        void PlaceWeaponPickup(WeaponPickup pickup);
        void Slowdown(float Time);
        void Slowdown(float Time, float ShakeIntensity);
        void Reload(std::string Filename);
        void Update(Camera2D camera);
        void Clear();
        void Quit();
        void UnloadAssets();
};


#endif //BOUNCINGPLUS_GAME_H