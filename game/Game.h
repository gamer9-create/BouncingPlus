//
// Created by lalit on 8/26/2025.
//

#ifndef BOUNCINGPLUS_GAME_H
#define BOUNCINGPLUS_GAME_H

#include <raylib.h>
#include <nlohmann/json_fwd.hpp>

#include "managers/GameModeManager.h"
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
        // Timing, Speed, and Menu Management
        double GameTime;
        float GameSpeed;
        bool Paused;
        bool ShouldReturn;
        bool DebugDraw;

        // Level Information
        std::string CurrentLevelName;
        std::unordered_map<std::string, nlohmann::json> LevelData;

        // Managers
        TileManager MainTileManager;
        EntityManager MainEntityManager;
        CameraManager MainCameraManager;
        ParticleManager MainParticleManager;
        SoundManager MainSoundManager;
        GameModeManager MainGameModeManager;

        shared_ptr<Player> MainPlayer;
        std::vector<WeaponPickup> WeaponPickups;

        // Assets
        unordered_map<std::string, Texture2D> Textures;
        unordered_map<std::string, Shader> Shaders;
        unordered_map<std::string, Weapon> Weapons;

        // Extra Assets
        std::vector<std::string> EnemyWeaponNamesList;
        std::vector<std::string> BannedWeaponDrops;

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
        float GetGameDeltaTime();
        double GetGameTime();
};


#endif //BOUNCINGPLUS_GAME_H