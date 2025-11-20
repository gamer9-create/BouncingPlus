//
// Created by lalit on 8/26/2025.
//

#ifndef BOUNCINGPLUS_GAME_H
#define BOUNCINGPLUS_GAME_H

#include <raylib.h>
#include <nlohmann/json_fwd.hpp>
#include "managers/CameraManager.h"
#include "../entities/Entity.h"
#include "../entities/EntityManager.h"
#include "managers/ParticleManager.h"
#include "../entities/subentities/Player.h"
#include "managers/SoundManager.h"
#include "../level/TileManager.h"
#include "managers/UIManager.h"
#include "../entities/Weapons.h"

using namespace std;

class Game {

    float SlowdownTime;
    float MaxSlowdownTime;
    float SlowdownShakeIntensity;

    UIManager MainUIManager;
    void SetGameData();
    void ProcessSlowdownAnimation();
    void DisplayPickups();

    RenderTexture2D WeaponPickupTex;
    int uTextureSize;
    int uOutlineSize;
    int uOutlineColor;
    int uThreshold;

    public:
        bool UpgradeUI;
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
        std::vector<std::string> EnemyWeaponNamesList;
        std::vector<std::string> BannedWeaponDrops;
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
        void Update();
        void Clear();
        void Quit();
        void UnloadAssets();
};


#endif //BOUNCINGPLUS_GAME_H