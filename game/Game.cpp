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

Game::Game(Settings& GameSettings)
{
    this->GameSettings = &GameSettings;
    LevelData = this->GameSettings->LevelData;

    // init game services
    MainUIManager = UIManager(*this);
    GameTiles = TileManager(*this);
    Particles = ParticleManager(*this);
    GameCamera = CameraManager(*this);
    GameEntities = EntityManager(*this);
    GameSounds = SoundManager(*this);
    GameMode = GameModeManager(*this);
    GameResources = ResourceManager(*this);

    // profiler
    MainProfiler = Profiler(*this);

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
    FreezeZones = std::vector<std::pair<Rectangle, double>>();

    // extra stuff
    MainPlayer = nullptr;
    CurrentLevelName = "";
    DebugDraw = false;
    ShouldReturn = false;
    isReturning = false;

    SetGameData();
}

void Game::SetGameData() {
    GameResources.Load();

    uOutlineSize = GetShaderLocation(GameResources.Shaders["outline"], "outlineSize");
    uOutlineColor = GetShaderLocation(GameResources.Shaders["outline"], "outlineColor");
    uTextureSize = GetShaderLocation(GameResources.Shaders["outline"], "textureSize");
    uThreshold = GetShaderLocation(GameResources.Shaders["outline"], "threshold");

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
            GameCamera.ShakeCamera(SlowdownShakeIntensity);
            GameSounds.PlaySoundM("dash_hit", min(max(SlowdownShakeIntensity, 0.0f), 1.0f));
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

void Game::PlaceWeaponPickup(WeaponPickup Pickup) {
    for (std::string s : BannedWeaponDrops)
    {
        if (s == Pickup.Weapon)
            return;
    }
    Pickup.CreationTime = GetGameTime();
    WeaponPickups.push_back(Pickup);
}

void Game::DisplayPickups()
{
    std::erase_if(WeaponPickups, [&](WeaponPickup& pickup) {
            return pickup.PickedUp || GetGameTime() - pickup.CreationTime >= 45 || !GameResources.Weapons.contains(pickup.Weapon);
    });
    for (WeaponPickup& pickup : WeaponPickups)
    {
        // get floating offset
        float AnimationOffset = sin((GetGameTime() - pickup.CreationTime) * pickup.AnimationSpeed) * pickup.AnimationPower;
        Weapon& PickupWeapon = GameResources.Weapons.at(pickup.Weapon);
        std::string TexString = "placeholder";
        if (GameResources.Textures.contains(PickupWeapon.texture))
            TexString=PickupWeapon.texture;

        Vector2 siz = {(float)GameResources.Textures[TexString].width, (float)GameResources.Textures[TexString].height};
        siz = Vector2Normalize(siz);
        siz = Vector2Multiply(siz, {pickup.Radius*2.5f, pickup.Radius*2.5f});

        float outlineSize = 3.0f;
        float threshold = 0.5f;
        Color outlineColor = pickup.PickupColor;

        DrawCircle(pickup.Position.x, pickup.Position.y, pickup.Radius / 1.125f, ColorAlpha(BLACK, 0.2f));

        EndTextureMode();
        EndMode2D();
        BeginTextureMode(WeaponPickupTex);
        ClearBackground(BLANK);

        BeginBlendMode(BLEND_ALPHA_PREMULTIPLY);

        DrawTexturePro(GameResources.Textures[TexString], {
            0,
            0,
            (float)GameResources.Textures[TexString].width,
            (float)GameResources.Textures[TexString].height
        }, {
            WeaponPickupTex.texture.width/2.0f,WeaponPickupTex.texture.height/2.0f,
            siz.x,
            siz.y

        }, {siz.x / 2, siz.y / 2}, 0, WHITE);
        EndBlendMode();
        EndTextureMode();
        BeginTextureMode(GameCamera.CameraRenderTexture);
        BeginMode2D(GameCamera.RaylibCamera);

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
        BeginShaderMode(GameResources.Shaders["outline"]);
        SetShaderValue(GameResources.Shaders["outline"], uTextureSize, &texSize, SHADER_UNIFORM_VEC2);
        SetShaderValue(GameResources.Shaders["outline"], uThreshold, &threshold, SHADER_UNIFORM_FLOAT);
        SetShaderValue(GameResources.Shaders["outline"], uOutlineSize, &outlineSize, SHADER_UNIFORM_FLOAT);
        SetShaderValue(GameResources.Shaders["outline"], uOutlineColor, &outlineColorRGB, SHADER_UNIFORM_VEC4);
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
        if (DistanceToPickup <= pickup.Radius && pickup.LeftOwner)
        {
            pickup.PickedUp = MainPlayer->MainWeaponsSystem.GiveWeapon(pickup.Weapon, pickup.Ammo);
        } else if (DistanceToPickup > pickup.Radius && !pickup.LeftOwner)
            pickup.LeftOwner = true;
    }
}

void Game::ProcessFreezeZones()
{
    std::erase_if(FreezeZones, [this](std::pair<Rectangle, double> rec) {
        return GetGameTime() - rec.second >= 45;
        });
    for (std::pair<Rectangle, double> &rec : FreezeZones)
    {
        float Alpha = 1.0f;
        if (GetGameTime() - rec.second <= 1.0f)
            Alpha = GetGameTime() - rec.second;
        if (GetGameTime() - rec.second >= 44.0f)
            Alpha = 45.0f - (GetGameTime() - rec.second);
        DrawRectangleRec(rec.first, ColorAlpha(BLUE, Alpha));
        DrawRectangleRec({rec.first.x + 4, rec.first.y + 4, rec.first.width - 8, rec.first.height - 8}, ColorAlpha(SKYBLUE, Alpha));
    }
}

void Game::Update() {

    if (IsKeyPressed(KEY_M))
        Paused = !Paused;
    if (!IsWindowFocused())
        Paused=true;

    if (!Paused) {
        if (!IsCursorHidden())
            HideCursor();

        if (IsKeyPressed(KEY_X))
            DebugDraw = !DebugDraw;

        GameTime += GetGameDeltaTime();

        // shader stuff
        if (!DebugDraw)
            GameCamera.ShaderDraw = false;
        if (IsKeyPressed(KEY_C) && DebugDraw)
            GameCamera.ShaderDraw = !GameCamera.ShaderDraw;
        if (IsKeyDown(KEY_B) && GameCamera.ShaderDraw)
            GameCamera.ShaderPixelPower += 10 * GetGameDeltaTime();
        if (IsKeyDown(KEY_N) && GameCamera.ShaderDraw)
            GameCamera.ShaderPixelPower -= 10 * GetGameDeltaTime();


        if (IsKeyPressed(KEY_E))
        {
            if (GameMode.WonLevel)
                isReturning=true;
            else if ((MainPlayer->Health <= 0 || MainPlayer->ShouldDelete) && !CurrentLevelName.empty())
                Reload(CurrentLevelName);
        }

        GameCamera.Begin();
        ProcessSlowdownAnimation();
        ProcessFreezeZones();
        DisplayPickups();

        MainProfiler.ProfilerLog("tiles");
        GameTiles.Update();

        MainProfiler.ProfilerLog("particles");
        Particles.Update();

        MainProfiler.ProfilerLog("entities");
        GameEntities.Update();

        MainProfiler.ProfilerLog("sound");
        GameSounds.Update();

        MainProfiler.ProfilerLog("gameplay");
        GameMode.Update();

        std::map<std::string, double> times = MainProfiler.Finish();

        if (IsKeyDown(KEY_C))
        {
            int i = 0;
            for (auto [name,val] : times)
            {
                DrawText((name + ", " + to_string((int)(val * 1000.0f)) + "ms").c_str(), 250 + GameCamera.RaylibCamera.target.x, 150 + (i * 25)+ GameCamera.RaylibCamera.target.y, 25, RED);
                i++;
            }
        }



        GameCamera.End();

    } else if (IsCursorHidden())
    {
        ShowCursor();
    }

    GameCamera.Display();
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

struct Vector2i
{
    int x;
    int y;
};

std::pair<bool, Vector2> Game::RayCastPoint(Vector2 Origin, Vector2 Target, bool Debug)
{
    Vector2 vRayStart = Vector2{Origin.x / GameTiles.TileSize, Origin.y / GameTiles.TileSize};
    Vector2 vRayTarget = Vector2{Target.x / GameTiles.TileSize, Target.y / GameTiles.TileSize};
    Vector2 vRayDir = Vector2Normalize(vRayTarget - vRayStart);

    Vector2 vRayUnitStepSize = { sqrt(1 + (vRayDir.y / vRayDir.x) * (vRayDir.y / vRayDir.x)), sqrt(1 + (vRayDir.x / vRayDir.y) * (vRayDir.x / vRayDir.y)) };
    Vector2i vMapCheck = {(int)vRayStart.x, (int)vRayStart.y};
    Vector2 vRayLength1D = {0, 0};
    Vector2i vStep = {0, 0};

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

    // Perform "Walk" until collision or range check
    bool bTileFound = false;
    float fMaxDistance = Vector2Distance(vRayStart, vRayTarget);
    float fDistance = 0.0f;
    while (!bTileFound && fDistance < fMaxDistance)
    {
        Vector2i vMapCheck_Copy = vMapCheck;
        Vector2 vRayLength1D_Copy = vRayLength1D;

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

        if (fDistance >= fMaxDistance)
        {
            vMapCheck = vMapCheck_Copy;
            fDistance = fMaxDistance;
            vRayLength1D = vRayLength1D_Copy;
            break;
        }

        // Test tile at new test point
        if (vMapCheck.x >= 0 && vMapCheck.x < GameTiles.MapWidth && vMapCheck.y >= 0 && vMapCheck.y < GameTiles.MapHeight)
        {
            if (GameTiles.TileTypes[GameTiles.GetTileAt({(float)vMapCheck.x,(float)vMapCheck.y})] == WallTileType)
            {
                bTileFound = true;
                break;
            }
        }
    }

    // Calculate intersection location
    Vector2 vIntersection = Origin + Vector2Normalize(Origin - Target) * fDistance * GameTiles.TileSize;

    if (Debug)
        cout << fDistance << " " << fMaxDistance << " " << vIntersection.x << " " << vIntersection.y << endl;

    if (DebugDraw)
    {
        Vector2 l = Origin + Vector2Normalize(Origin - Target) * -fDistance * GameTiles.TileSize;
        DrawLine(l.x, l.y,vRayStart.x*GameTiles.TileSize,vRayStart.y*GameTiles.TileSize,RED);
    }

    return std::make_pair(!bTileFound, vIntersection);
}

bool Game::RayCast(Vector2 Origin, Vector2 Target, bool Debug) {
    return RayCastPoint(Origin,Target,Debug).first;
}

void Game::Clear() {
    Paused = false;
    ShouldReturn = false;
    isReturning = false;
    GameTime = 0;
    GameScore = 0;
    GameEntities.Clear();
    BannedWeaponDrops.clear();
    EnemyRoleWeapons.clear();
    CurrentLevelName.clear();
    WeaponPickups.clear();
    GameTiles.Clear();
    FreezeZones.clear();
    Particles.Clear();
    GameSounds.Clear();
    GameCamera.Clear();
    GameMode.Clear();
    MainPlayer.reset();
}

void Game::Reload(std::string MapName) {

    Clear();

    CurrentLevelName = MapName;

    for (std::string s : LevelData[MapName]["game"]["banned_spawn_weapons"])
        BannedWeaponDrops.emplace_back(s);
    EnemyRoleWeapons= LevelData[MapName]["enemy_weapons"].get<unordered_map<std::string, std::string>>();

    GameTiles.ReadMapDataFile("assets\\maps\\" + CurrentLevelName + "\\map_data.csv");
    GameMode.PrepareGameMode(LevelData[MapName]);

    MainPlayer = make_shared<Player>(GameTiles.PlayerSpawnPosition.x,
                                     GameTiles.PlayerSpawnPosition.y, LevelData[MapName]["player"]["starting_speed"],
                                     GameResources.Textures["player"], *this);
    MainPlayer->MaxHealth = LevelData[MapName]["player"]["starting_health"];
    MainPlayer->Health = LevelData[MapName]["player"]["starting_health"];
    GameEntities.AddEntity(PlayerType, MainPlayer);
}

void Game::UnloadAssets() {
    EnemyRoleWeapons.clear();
    WeaponPickups.clear();
    GameResources.Weapons.clear();
    BannedWeaponDrops.clear();
    UnloadRenderTexture(WeaponPickupTex);
}

void Game::Quit() {
    Clear();
    GameEntities.Quit();
    GameTiles.Quit();
    MainUIManager.Quit();
    Particles.Quit();
    GameCamera.Quit();
    GameSounds.Quit();
    GameResources.Quit();
    GameMode.Quit();
    UnloadAssets();
}