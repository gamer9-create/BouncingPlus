//
// Created by lalit on 8/26/2025.
//

#include "Game.h"

#include <iostream>
#include <raymath.h>

#include "raylib.h"
#include "Weapons.h"
#include "UIManager.h"

using namespace std;

Game::Game() {

    // init game services
    MainUIManager = UIManager(*this);
    MainTileManager = TileManager(*this);
    MainParticleManager = ParticleManager(*this);
    MainCameraManager = CameraManager(*this);
    MainEntityManager = EntityManager(*this);

    // game speed & timing
    GameSpeed = 1.0f;
    SlowdownTime = 0;
    MaxSlowdownTime = 0;

    Paused = false;

    // resource maps
    Textures = std::unordered_map<std::string, Texture2D>();
    Weapons = std::unordered_map<std::string, Weapon>();

    // extra stuff
    MainPlayer = nullptr;
    current_map_filename = "";
    DebugDraw = false;

    SetGameData();
}

void Game::SetGameData() {
    Textures.insert({"player", LoadTexture("assets/img/player.png")});
    Textures.insert({"enemy", LoadTexture("assets/img/enemy.png")});
    Textures.insert({"bullet", LoadTexture("assets/img/bullet.png")});
    Textures.insert({"sword", LoadTexture("assets/img/sword.png")});
    Textures.insert({"shotgun", LoadTexture("assets/img/shotgun.png")});
    Textures.insert({"armor_overlay", LoadTexture("assets/img/armor_overlay.png")});
    Sounds.insert({"dash_hit", LoadSound("assets/sounds/dash_hit.wav")});
    Sounds.insert({"death", LoadSound("assets/sounds/death.wav")});
    Sounds.insert({"dash", LoadSound("assets/sounds/dash.mp3")});
    Sounds.insert({"shotgun", LoadSound("assets/sounds/shotgun.wav")});
    Weapons.insert({"Default Gun", {false, false, false, 400, 1.0f, 20, 0.2, 0.0f, 0.0f, 1, 0, "", ""}});
    Weapons.insert({"Player Gun", {false, false, false, 1000, 1.0f, 20, 0.2, 0.0f, 0.0f, 1, 0, "", ""}});
    Weapons.insert({"Shotgun", {false, true, true, 1600, 4.0f, 10, 1.0f, 40.0f, 150.0f, 10, 0.5f, "shotgun", "shotgun"}});
    Weapons.insert({"Sword", {true, false, false, 0.4, 1.0f, 35, 0.45f, 90.0f, 350.0f, -1, 0, "sword", ""}});
    Weapons.insert({"Enemy Sword", {true, false, false, 0.4f, 1.0f, 35.0f, 0.55f, 90.0f, 204.0f, -1, 0, "sword", ""}});
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
        float Percent = SlowdownTime / MaxSlowdownTime;
        if (Percent < 0.5)
            GameSpeed = Lerp(1.0f, 0.1f, Percent-0.5f);
        else
            GameSpeed = Lerp(0.1f, 1.0f, Percent-0.5f);
        if (SlowdownShakeIntensity > 0 && Percent < 0.5f) {
            MainCameraManager.ShakeCamera(SlowdownShakeIntensity);
            SetSoundVolume(Sounds["dash_hit"], min(max(SlowdownShakeIntensity, 0.0f), 1.0f));
            PlaySound(Sounds["dash_hit"]);
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

void Game::Update(Camera2D camera) {

    if (IsKeyPressed(KEY_M))
        Paused = !Paused;

    if (!Paused) {
        if (IsKeyPressed(KEY_X))
            DebugDraw = !DebugDraw;

        if ((MainPlayer->Health <= 0 || MainPlayer->ShouldDelete) && IsKeyPressed(KEY_E) && !current_map_filename.empty())
            Reload(current_map_filename);

        MainCameraManager.Begin(camera);

        ProcessSlowdownAnimation();
        MainTileManager.Update();
        MainEntityManager.Update();

        MainCameraManager.End();
    }

    MainCameraManager.Display();
    MainUIManager.GameUI();
}

bool Game::RayCast(Vector2 origin, Vector2 target) {

    Vector2 vRayStart = origin;
    Vector2 vRayDir = Vector2Normalize(target - origin);
    Vector2 vRayUnitStepSize = { sqrt(1 + (vRayDir.y / vRayDir.x) * (vRayDir.y / vRayDir.x)), sqrt(1 + (vRayDir.x / vRayDir.y) * (vRayDir.x / vRayDir.y)) };
    Vector2 vMapCheck = origin;
    Vector2 vRayLength1D;
    Vector2 vStep;

    // Establish Starting Conditions
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

        // Test tile at new test point
        if (vMapCheck.x >= 0 && vMapCheck.x < MainTileManager.MapWidth*MainTileManager.TileSize && vMapCheck.y >= 0 && vMapCheck.y < MainTileManager.MapHeight*MainTileManager.TileSize)
        {
            std::string s = to_string((int)(vMapCheck.x/MainTileManager.TileSize)) + " " + to_string((int)(vMapCheck.y/MainTileManager.TileSize));
            int g = MainTileManager.Map[s];
            // cout << s << " " << g << endl;

            if (g >0 && g < 3)
            {
                bTileFound = true;
            }
        }
    }

    /*
    // Calculate intersection location
    Vector2 vIntersection;
    if (bTileFound)
    {
        vIntersection = vRayStart + vRayDir * fDistance;
    }
    */
    return !bTileFound;
}


bool Game::RayCastSpecifiedPrecision(Vector2 origin, Vector2 target, float Precision) { // RayCasting (old)function

    if (Precision <= 0)
        Precision = Vector2Distance(origin, target) / 15.0f;

    // Init starting positions
    float ray_x = origin.x;
    float ray_y = origin.y;
    auto start_distance = static_cast<float>(sqrt(pow(ray_x - target.x, 2) + pow(ray_y - target.y, 2)));

    // Getting the step distance

    const float step_x = ((target.x-ray_x) / start_distance) * Precision;
    const float step_y = ((target.y-ray_y) / start_distance) * Precision;

    // Getting the side we are on relative to the target

    bool x_side = (target.x - ray_x) < 0;
    bool y_side = (target.y - ray_y) < 0;

    // marching through the world, check if we collide with something
    while (sqrt(pow(ray_x - target.x, 2) + pow(ray_y - target.y, 2)) > 2) {
        ray_x += step_x;
        ray_y += step_y;
        if (DebugDraw)
            DrawCircle((int)ray_x-MainCameraManager.CameraPosition.x, (int)ray_y-MainCameraManager.CameraPosition.y, 2, RED);
        std::string coord = std::to_string((int) (ray_x / MainTileManager.TileSize)) + " " + std::to_string((int) (ray_y / MainTileManager.TileSize));
        // if we collide with a wall, raycast failed
        if (int tile_id = MainTileManager.Map[coord]; tile_id > 0 && tile_id <= 2) {
            return false;
        }
        bool curr_x_side = (target.x - ray_x) < 0;
        bool curr_y_side = (target.y - ray_y) < 0;
        if (curr_x_side != x_side || curr_y_side != y_side) { // Have we traveled beyond the target? havent found anything? If so, exit!!
            return true;
        }
    }
    return true;
}

void Game::Clear() {

    Paused = false;
    MainTileManager.Reset();
    current_map_filename.clear();
    MainEntityManager.Reset();
    MainCameraManager.Reset();
    MainPlayer.reset();
}

void Game::Reload(std::string Filename) {

    Clear();

    current_map_filename = Filename;

    MainTileManager.ReadMapDataFile(Filename + "\\map_data.csv");

    MainPlayer = make_shared<Player>((static_cast<float>(MainTileManager.MapWidth) * MainTileManager.TileSize) / 2.0f,
                                     (static_cast<float>(MainTileManager.MapHeight) * MainTileManager.TileSize) / 2.0f, 350.0f,
                                     Textures["player"], *this);
    MainEntityManager.AddEntity(PlayerType, MainPlayer);
}

void Game::Quit() {
    Clear();
    MainTileManager.Quit();
    MainUIManager.Quit();
    MainParticleManager.Quit();
    MainCameraManager.Quit();
    MainEntityManager.Quit();
    for (auto [name,value] : Textures) {
        UnloadTexture(value);
    }
    for (auto [name,value] : Sounds) {
        UnloadSound(value);
    }
}