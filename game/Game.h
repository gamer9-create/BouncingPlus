//
// Created by lalit on 8/26/2025.
//

#ifndef BOUNCINGPLUS_GAME_H
#define BOUNCINGPLUS_GAME_H

#include <raylib.h>
#include <nlohmann/json_fwd.hpp>

#include "Profiler.h"
#include "managers/GameModeManager.h"
#include "managers/CameraManager.h"
#include "../entities/Entity.h"
#include "managers/EntityManager.h"
#include "managers/ParticleManager.h"
#include "../entities/subentities/Player.h"
#include "managers/SoundManager.h"
#include "../level/TileManager.h"
#include "ui/UIManager.h"
#include "../entities/Weapons.h"
#include "managers/ResourceManager.h"
#include "saves/Settings.h"

using namespace std;

class Game {

    float SlowdownTime;
    float MaxSlowdownTime;
    float SlowdownShakeIntensity;

    UIManager MainUIManager;
    Profiler MainProfiler;
    void SetGameData();
    void ProcessSlowdownAnimation();
    void DisplayPickups();

    RenderTexture2D WeaponPickupTex;
    int uTextureSize;
    int uOutlineSize;
    int uOutlineColor;
    int uThreshold;

    public:
        Settings* GameSettings;

        // Timing, Speed, and Menu Management
        double GameTime;
        float GameSpeed;
        bool Paused;
        bool isReturning;
        bool ShouldReturn;
        bool DebugDraw;
        float GameScore;

        // Level Information
        std::string CurrentLevelName;
        std::map<std::string, nlohmann::json> LevelData;

        // Managers
        TileManager MainTileManager;
        EntityManager MainEntityManager;
        CameraManager MainCameraManager;
        ParticleManager MainParticleManager;
        SoundManager MainSoundManager;
        GameModeManager MainGameModeManager;
        ResourceManager MainResourceManager;

        shared_ptr<Player> MainPlayer;
        std::vector<WeaponPickup> WeaponPickups;

        // Extra Assets
        unordered_map<std::string, std::string> EnemyRoleWeapons;
        std::vector<std::string> BannedWeaponDrops;

        Game(Settings& GameSettings);
        std::pair<bool, Vector2> RayCastPoint(Vector2 Origin, Vector2 Target);
        bool RayCast(Vector2 Origin, Vector2 Target);
        void PlaceWeaponPickup(WeaponPickup Pickup);
        void Slowdown(float Time);
        void Slowdown(float Time, float ShakeIntensity);
        void Reload(std::string Filename);
        void Update();
        void Clear();
        void Quit();
        void UnloadAssets();
        float GetGameDeltaTime();
        double GetGameTime();
};


#endif //BOUNCINGPLUS_GAME_H