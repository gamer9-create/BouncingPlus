//
// Created by lalit on 8/26/2025.
//

#include "Game.h"

#include <iostream>
#include <raymath.h>

#include "raylib.h"
#include "../entities/Weapons.h"
#include "../entities/Powerups.h"
#include "ui/UIManager.h"

#include <filesystem>
#include <fstream>
#include <iso646.h>

#include "../level/LevelLoader.h"
namespace fs = std::filesystem;

using namespace std;

Game::Game(std::map<std::string, nlohmann::json> json)
{
    // init game services
    MainUIManager = UIManager(*this);
    MainTileManager = TileManager(*this);
    MainParticleManager = ParticleManager(*this);
    MainCameraManager = CameraManager(*this);
    MainEntityManager = EntityManager(*this);
    MainSoundManager = SoundManager(*this);
    MainGameModeManager = GameModeManager(*this);
    MainResourceManager = ResourceManager(*this);

    // profiler
    profiler = Profiler(*this);

    // game speed & timing
    GameSpeed = 1.0f;
    SlowdownTime = 0;
    GameTime = 0.0f;
    MaxSlowdownTime = 0;
    GameScore = 0;

    Paused = false;

    BannedWeaponDrops = std::vector<std::string>();
    EnemyRoleWeapons = unordered_map<std::string, std::string>();
    WeaponPickups = std::vector<WeaponPickup>();

    // extra stuff
    MainPlayer = nullptr;
    CurrentLevelName = "";
    LevelData = json;
    DebugDraw = false;
    ShouldReturn = false;
    isReturning = false;

    SetGameData();
}

void Game::SetGameData() {
    MainResourceManager.Load();

    uOutlineSize = GetShaderLocation(MainResourceManager.Shaders["outline"], "outlineSize");
    uOutlineColor = GetShaderLocation(MainResourceManager.Shaders["outline"], "outlineColor");
    uTextureSize = GetShaderLocation(MainResourceManager.Shaders["outline"], "textureSize");
    uThreshold = GetShaderLocation(MainResourceManager.Shaders["outline"], "threshold");

    WeaponPickupTex = LoadRenderTexture(150, 150);

    BannedWeaponDrops.emplace_back("Default Gun");
    BannedWeaponDrops.emplace_back("Player Gun");
}

float Game::GetGameDeltaTime()
{
    return GetFrameTime() * GameSpeed;
}

double Game::GetGameTime()
{
    return GameTime;
}

void Game::Slowdown(float Time) {
    SlowdownTime = Time;
    MaxSlowdownTime = Time;
}

void Game::Slowdown(float Time, float CrashIntensity) {
    SlowdownTime = Time;
    MaxSlowdownTime = Time;
    SlowdownShakeIntensity = CrashIntensity;
}

void Game::ProcessSlowdownAnimation() {
    if (SlowdownTime > 0 && MaxSlowdownTime > 0) {
        GameSpeed = 0.0f;
        float Percent = SlowdownTime / MaxSlowdownTime;
        if (Percent >= 0.1)
        {
            if (Percent < 0.5)
                GameSpeed = Lerp(1.0f, 0.1f, Percent / 0.5f);
            else
                GameSpeed = Lerp(0.1f, 1.0f, (Percent-0.5f) / 0.5f);
        }
        if (SlowdownShakeIntensity > 0 && Percent < 0.5f) {
            MainCameraManager.ShakeCamera(SlowdownShakeIntensity);
            MainSoundManager.PlaySoundM("dash_hit", min(max(SlowdownShakeIntensity, 0.0f), 1.0f));
            SlowdownShakeIntensity = 0;
        }
        SlowdownTime -= GetFrameTime();
    } else {
        SlowdownTime = 0;
        MaxSlowdownTime = 0;
        SlowdownShakeIntensity = 0;
        GameSpeed = 1.0f;
    }
}

void Game::PlaceWeaponPickup(WeaponPickup pickup) {
    for (std::string s : BannedWeaponDrops)
    {
        if (s == pickup.Weapon)
            return;
    }
    pickup.CreationTime = GetGameTime();
    WeaponPickups.push_back(pickup);
}

void Game::DisplayPickups()
{
    std::erase_if(WeaponPickups, [&](WeaponPickup& pickup) {
            return pickup.PickedUp || GetGameTime() - pickup.CreationTime >= 45 || !MainResourceManager.Weapons.contains(pickup.Weapon);
    });
    for (WeaponPickup& pickup : WeaponPickups)
    {
        // get floating offset
        float AnimationOffset = sin((GetGameTime() - pickup.CreationTime) * pickup.AnimationSpeed) * pickup.AnimationPower;
        Weapon& PickupWeapon = MainResourceManager.Weapons.at(pickup.Weapon);
        std::string TexString = "placeholder";
        if (MainResourceManager.Textures.contains(PickupWeapon.texture))
            TexString=PickupWeapon.texture;

        Vector2 siz = {(float)MainResourceManager.Textures[TexString].width, (float)MainResourceManager.Textures[TexString].height};
        siz = Vector2Normalize(siz);
        siz = Vector2Multiply(siz, {pickup.Radius*2.5f, pickup.Radius*2.5f});

        float outlineSize = 3.0f;
        float threshold = 0.5f;
        Color outlineColor = pickup.PickupColor;

        DrawCircle(pickup.Position.x, pickup.Position.y, min(siz.x, siz.y)/2.5f, ColorAlpha(BLACK, 0.2f));

        EndTextureMode();
        EndMode2D();
        BeginTextureMode(WeaponPickupTex);
        ClearBackground(BLANK);

        BeginBlendMode(BLEND_ALPHA_PREMULTIPLY);

        DrawTexturePro(MainResourceManager.Textures[TexString], {
            0,
            0,
            (float)MainResourceManager.Textures[TexString].width,
            (float)MainResourceManager.Textures[TexString].height
        }, {
            WeaponPickupTex.texture.width/2.0f,WeaponPickupTex.texture.height/2.0f,
            siz.x,
            siz.y

        }, {siz.x / 2, siz.y / 2}, 0, WHITE);
        EndBlendMode();
        EndTextureMode();
        BeginTextureMode(MainCameraManager.CameraRenderTexture);
        BeginMode2D(MainCameraManager.RaylibCamera);

        Vector2 texSize = {(float)WeaponPickupTex.
            texture.width,
            (float)WeaponPickupTex.texture.height};
        float outlineColorRGB[4] = {
            (float)outlineColor.r / 255.0f,
                (float)outlineColor.g / 255.0f,
                (float)outlineColor.b / 255.0f,
                    (float)outlineColor.a / 255.0f
        };

        BeginBlendMode(BLEND_ALPHA_PREMULTIPLY);
        BeginShaderMode(MainResourceManager.Shaders["outline"]);
        SetShaderValue(MainResourceManager.Shaders["outline"], uTextureSize, &texSize, SHADER_UNIFORM_VEC2);
        SetShaderValue(MainResourceManager.Shaders["outline"], uThreshold, &threshold, SHADER_UNIFORM_FLOAT);
        SetShaderValue(MainResourceManager.Shaders["outline"], uOutlineSize, &outlineSize, SHADER_UNIFORM_FLOAT);
        SetShaderValue(MainResourceManager.Shaders["outline"], uOutlineColor, &outlineColorRGB, SHADER_UNIFORM_VEC4);
        DrawTexturePro(WeaponPickupTex.texture, {0,0,(float)WeaponPickupTex.texture.width,
            (float)-WeaponPickupTex.texture.height
        }, {pickup.Position.x,
            pickup.Position.y - AnimationOffset,
            (float)WeaponPickupTex.texture.width,
                (float)WeaponPickupTex.texture.height},
            {WeaponPickupTex.texture.width/2.0f,WeaponPickupTex.texture.height/2.0f}, 0, WHITE);
        EndBlendMode();
        EndShaderMode();

        if (DebugDraw)
            DrawCircleV({pickup.Position.x,
            pickup.Position.y - AnimationOffset}, pickup.Radius, ColorAlpha(RED, 0.5f));

        // get distance
        float DistanceToPickup = Vector2Distance(pickup.Position, {
            MainPlayer->BoundingBox.x,
            MainPlayer->BoundingBox.y
        });

        // in range?
        if (DistanceToPickup <= pickup.Radius)
        {
            pickup.PickedUp = MainPlayer->weaponsSystem.GiveWeapon(pickup.Weapon, pickup.Ammo);
        }
    }
}

void Game::Update() {

    if (IsKeyPressed(KEY_M))
        Paused = !Paused;
    if (!IsWindowFocused())
        Paused=true;

    if (!Paused) {
        if (IsKeyPressed(KEY_X))
            DebugDraw = !DebugDraw;

        GameTime += GetGameDeltaTime();

        // shader stuff
        if (!DebugDraw)
            MainCameraManager.ShaderDraw = false;
        if (IsKeyPressed(KEY_C) && DebugDraw)
            MainCameraManager.ShaderDraw = !MainCameraManager.ShaderDraw;
        if (IsKeyDown(KEY_B) && MainCameraManager.ShaderDraw)
            MainCameraManager.ShaderPixelPower += 10 * GetGameDeltaTime();
        if (IsKeyDown(KEY_N) && MainCameraManager.ShaderDraw)
            MainCameraManager.ShaderPixelPower -= 10 * GetGameDeltaTime();


        if (IsKeyPressed(KEY_E))
        {
            //if (MainEntityManager.Entities[EnemyType].size() <= 0)
            //    ShouldReturn = true;
            if ((MainPlayer->Health <= 0 || MainPlayer->ShouldDelete) && !CurrentLevelName.empty())
                Reload(CurrentLevelName);
        }

        MainCameraManager.Begin();
        ProcessSlowdownAnimation();

        profiler.log("tiles");
        MainTileManager.Update();

        profiler.log("particles");
        MainParticleManager.Update();

        profiler.log("entities");
        MainEntityManager.Update();

        profiler.log("sound");
        MainSoundManager.Update();

        profiler.log("gameplay");
        MainGameModeManager.Update();

        std::map<std::string, double> times = profiler.finish();

        if (IsKeyDown(KEY_C))
        {
            int i = 0;
            for (auto [name,val] : times)
            {
                DrawText((name + ", " + to_string((int)(val * 1000.0f)) + "ms").c_str(), 250 + MainCameraManager.RaylibCamera.target.x, 150 + (i * 25)+ MainCameraManager.RaylibCamera.target.y, 25, RED);
                i++;
            }
        }

        DisplayPickups();
        MainCameraManager.End();

    }

    MainCameraManager.Display();
    MainUIManager.GameUI();

    if (isReturning)
    {
        MainUIManager.StartingBlackScreenTrans = 0;
        MainUIManager.EndBlackScreenTrans += 0.65f * GetFrameTime();
        if (MainUIManager.EndBlackScreenTrans >= 0.9f)
            ShouldReturn = true;
    } else
        MainUIManager.EndBlackScreenTrans = 0;
    if (ShouldReturn)
    {
        MainUIManager.StartingBlackScreenTrans = 1.0f;
        MainUIManager.EndBlackScreenTrans = 0.0f;
    }
}

std::pair<bool, Vector2> Game::RayCastPoint(Vector2 origin, Vector2 target)
{
    Vector2 vRayStart = origin;
    Vector2 vRayDir = Vector2Normalize(target - origin);
    Vector2 vRayUnitStepSize = { sqrt(1 + (vRayDir.y / vRayDir.x) * (vRayDir.y / vRayDir.x)), sqrt(1 + (vRayDir.x / vRayDir.y) * (vRayDir.x / vRayDir.y)) };
    Vector2 vMapCheck = origin;
    Vector2 vRayLength1D;
    Vector2 vStep;

    if (vRayDir.x < 0)
    {
        vStep.x = -1;
        vRayLength1D.x = (vRayStart.x - float(vMapCheck.x)) * vRayUnitStepSize.x;
    }
    else
    {
        vStep.x = 1;
        vRayLength1D.x = (float(vMapCheck.x + 1) - vRayStart.x) * vRayUnitStepSize.x;
    }

    if (vRayDir.y < 0)
    {
        vStep.y = -1;
        vRayLength1D.y = (vRayStart.y - float(vMapCheck.y)) * vRayUnitStepSize.y;
    }
    else
    {
        vStep.y = 1;
        vRayLength1D.y = (float(vMapCheck.y + 1) - vRayStart.y) * vRayUnitStepSize.y;
    }

    bool bTileFound = false;
    float fMaxDistance = Vector2Distance(origin,target);
    float fDistance = 0.0f;
    while (!bTileFound && fDistance < fMaxDistance)
    {
        // Walk along shortest path
        if (vRayLength1D.x < vRayLength1D.y)
        {
            vMapCheck.x += vStep.x;
            fDistance = vRayLength1D.x;
            vRayLength1D.x += vRayUnitStepSize.x;
        }
        else
        {
            vMapCheck.y += vStep.y;
            fDistance = vRayLength1D.y;
            vRayLength1D.y += vRayUnitStepSize.y;
        }

        if (vMapCheck.x >= 0 && vMapCheck.x < MainTileManager.MapWidth*MainTileManager.TileSize && vMapCheck.y >= 0 && vMapCheck.y < MainTileManager.MapHeight*MainTileManager.TileSize)
        {
            std::string s = to_string((int)(vMapCheck.x/MainTileManager.TileSize)) + " " + to_string((int)(vMapCheck.y/MainTileManager.TileSize));
            int g = MainTileManager.Map[s];
            if (MainTileManager.TileTypes[g]==WallTileType)
            {
                bTileFound = true;
                break;
            }
        }
    }

    if (DebugDraw)
        DrawLine(origin.x,origin.y,vMapCheck.x,vMapCheck.y,RED);

    return std::pair(!bTileFound, vMapCheck);
}

bool Game::RayCast(Vector2 origin, Vector2 target) {
    return RayCastPoint(origin,target).first;
}

void Game::Clear() {
    Paused = false;
    ShouldReturn = false;
    isReturning = false;
    GameTime = 0;
    GameScore = 0;
    BannedWeaponDrops.clear();
    EnemyRoleWeapons.clear();
    CurrentLevelName.clear();
    WeaponPickups.clear();
    MainTileManager.Clear();
    MainParticleManager.Clear();
    MainEntityManager.Clear();
    MainSoundManager.Clear();
    MainCameraManager.Clear();
    MainGameModeManager.Clear();
    MainPlayer.reset();
}

void Game::Reload(std::string MapName) {

    Clear();

    CurrentLevelName = MapName;

    for (std::string s : LevelData[MapName]["game"]["banned_spawn_weapons"])
        BannedWeaponDrops.emplace_back(s);
    EnemyRoleWeapons= LevelData[MapName]["enemy_weapons"].get<unordered_map<std::string, std::string>>();

    MainTileManager.ReadMapDataFile("assets\\maps\\" + CurrentLevelName + "\\map_data.csv");
    MainGameModeManager.PrepareGameMode(LevelData[MapName]);

    MainPlayer = make_shared<Player>(MainTileManager.PlayerSpawnPosition.x,
                                     MainTileManager.PlayerSpawnPosition.y, LevelData[MapName]["player"]["starting_speed"],
                                     MainResourceManager.Textures["player"], *this);
    MainPlayer->MaxHealth = LevelData[MapName]["player"]["starting_health"];
    MainPlayer->Health = LevelData[MapName]["player"]["starting_health"];
    MainEntityManager.AddEntity(PlayerType, MainPlayer);
}

void Game::UnloadAssets() {
    EnemyRoleWeapons.clear();
    WeaponPickups.clear();
    MainResourceManager.Weapons.clear();
    BannedWeaponDrops.clear();
    UnloadRenderTexture(WeaponPickupTex);
}

void Game::Quit() {
    Clear();
    MainTileManager.Quit();
    MainUIManager.Quit();
    MainParticleManager.Quit();
    MainCameraManager.Quit();
    MainSoundManager.Quit();
    MainResourceManager.Quit();
    MainEntityManager.Quit();
    MainGameModeManager.Quit();
    UnloadAssets();
}