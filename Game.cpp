//
// Created by lalit on 8/26/2025.
//

#include "Game.h"

#include <iostream>
#include <raymath.h>

#include "raylib.h"
#include "Weapons.h"
#include "UI.h"

using namespace std;

Game::Game() {
    CameraPosition = Vector2(0.0f, 0.0f);
    CameraTarget = Vector2(0.0f, 0.0f);
    CameraPositionUnaffected = {0, 0};
    CameraSpeed = 20.0f;
    CameraZoom = 1.0f;
    CameraShakes = 0;
    GameSpeed = 1.0f;
    SlowdownTime = 0;
    MaxSlowdownTime = 0;
    CameraShakeIntensity = 0;
    CameraShakeOffset = {0, 0};
    CameraShakeTimer = GetTime();
    Ui = UI(*this);
    MainTileManager = TileManager(*this);
    MainParticleSystem = ParticleSystem(*this);
    Entities = std::unordered_map<EntityType, std::vector<shared_ptr<Entity> > >();
    Textures = std::unordered_map<std::string, Texture2D>();
    Weapons = std::unordered_map<std::string, Weapon>();
    PhysicsFPS = 240.0f;
    PhysicsAccumulator = 0;
    MainPlayer = nullptr;
    Paused = false;
    //GameRenderTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
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

    for (int i = 0; i < End; ++i) {
        Entities.insert({(EntityType) i, std::vector<shared_ptr<Entity>>()});
    }
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

void Game::Update() {

    //if (GameRenderTexture.texture.width != GetScreenWidth() || GameRenderTexture.texture.height != GetScreenHeight()) {
    //    UnloadRenderTexture(GameRenderTexture);
    //    GameRenderTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    //}

    //BeginTextureMode(GameRenderTexture);

    //ClearBackground(BLANK);

    if (!Paused) {
        if (SlowdownTime > 0 && MaxSlowdownTime > 0) {
            float Percent = SlowdownTime / MaxSlowdownTime;
            if (Percent < 0.5)
                GameSpeed = Lerp(1.0f, 0.1f, Percent-0.5f);
            else
                GameSpeed = Lerp(0.1f, 1.0f, Percent-0.5f);
            if (SlowdownShakeIntensity > 0 && Percent < 0.5f) {
                ShakeCamera(SlowdownShakeIntensity);
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

        PhysicsAccumulator += GetFrameTime();

        if (GetTime() - CameraShakeTimer >= 0.02f) {
            if (CameraShakes > 0) {
                CameraShakeOffset = {(float)GetRandomValue((int)(-50 * CameraShakeIntensity), (int)(50 * CameraShakeIntensity)), (float)GetRandomValue((int)(-50 * CameraShakeIntensity), (int)(50 * CameraShakeIntensity))};
                CameraShakes--;
            } else {
                CameraShakeOffset = {0, 0};
            }
            CameraShakeTimer = GetTime();
        }

        float TargetX = CameraTarget.x - CameraPositionUnaffected.x - (static_cast<float>(GetScreenWidth()) / 2.0f);
        float TargetY = CameraTarget.y - CameraPositionUnaffected.y - (static_cast<float>(GetScreenHeight()) / 2.0f);

        float ImportantVal = 20.0f * (static_cast<float>(GetFPS()) / 144.0f);
        if (ImportantVal != 0.0f) {
            CameraPositionUnaffected.x += TargetX / ImportantVal;
            CameraPositionUnaffected.y += TargetY / ImportantVal;
            CameraPosition = {CameraPositionUnaffected.x - CameraShakeOffset.x, CameraPositionUnaffected.y - CameraShakeOffset.y};
        }

        MainTileManager.Update();
        for (int e = 0; e < End; ++e) {
            std::vector<shared_ptr<Entity>>* array = &Entities[(EntityType)e];
            for (int i = 0; i < array->size(); i++) {
                if (shared_ptr<Entity> entity = array->at(i); entity != nullptr and !entity->ShouldDelete) {
                    entity->Update();
                }
            }
        }

        while (PhysicsAccumulator >= 1.0f/(PhysicsFPS*GameSpeed)) {
            for (int e = 0; e < End; ++e) {
                std::vector<shared_ptr<Entity>>* array = &Entities[(EntityType)e];
                for (int i = 0; i < array->size(); i++) {
                    if (shared_ptr<Entity> entity = array->at(i); entity != nullptr and !entity->ShouldDelete) {
                        entity->PhysicsUpdate(1.0f/PhysicsFPS);
                    }
                }
            }
            PhysicsAccumulator -= 1.0f/(PhysicsFPS*GameSpeed);
        }

        for (int e = 0; e < End; e++) {
            std::vector<shared_ptr<Entity>> *array = &Entities[(EntityType) e];
            int old_size = array->size();
            std::erase_if(*array, [this](shared_ptr<Entity> e) {
                if (e && e->ShouldDelete) {
                    if (e != MainPlayer) {
                        e.reset();
                    }
                    return true;
                }
                return false;
            });
            if (DebugDraw) {
                std::string f = to_string(e)+"/typa shit ive been on/ de old size(TALLY HALL DETECTED) " + to_string(old_size) + ", new size? (BANANA MAN IS COMING TO NUKE YOU) " + to_string(array->size());
                DrawText(f.c_str(), 500, 500, 10, WHITE);
            }
        }
        if ((MainPlayer->Health <= 0 || MainPlayer->ShouldDelete) && IsKeyPressed(KEY_E) && !current_map_filename.empty())
            Reload(current_map_filename);
    }

    //EndTextureMode();

    //DrawTexturePro(GameRenderTexture.texture, {0, 0, (float)GetScreenWidth(), (float)-GetScreenHeight()}, {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, {0,0},0, WHITE);

    Ui.GameUI();
}

void Game::ShakeCamera(float Intensity) {
    this->CameraShakeIntensity = Intensity;
    this->CameraShakeTimer = GetTime();
    this->CameraShakeOffset = {0, 0};
    this->CameraShakes = 14;
}

bool Game::RayCast(Vector2 origin, Vector2 target, float Precision) { // RayCasting function

    if (Precision <= 0)
        Precision = Vector2Distance(origin, target) / 40.0f;

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
            DrawCircle((int)ray_x-CameraPosition.x, (int)ray_y-CameraPosition.y, 2, RED);
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
    for (int e = 0; e < End; e++) {
        std::vector<shared_ptr<Entity>>* array = &Entities[(EntityType)e];
        for (int i = 0; i < array->size(); i++) {
            if (shared_ptr<Entity> entity = array->at(i); entity != nullptr) {
                entity.reset();
            }
        }
        array->clear();
    }
    CameraZoom = 1.0f;
    Paused = false;
    MainTileManager.Map.clear();
    current_map_filename.clear();
    CameraShakes = 0;
    MainPlayer.reset();
}

void Game::Reload(std::string Filename) {

    Clear();

    current_map_filename = Filename;

    MainTileManager.ReadMapDataFile(Filename + "\\map_data.csv");

    MainPlayer = make_shared<Player>((static_cast<float>(MainTileManager.MapWidth) * MainTileManager.TileSize) / 2.0f,
                                     (static_cast<float>(MainTileManager.MapHeight) * MainTileManager.TileSize) / 2.0f, 350.0f,
                                     Textures["player"], *this);
    Entities[PlayerType].push_back(MainPlayer);
}

void Game::Quit() {
    Clear();
    MainTileManager.Quit();
    Ui.Quit();
    MainParticleSystem.Quit();
    for (auto [name,value] : Textures) {
        UnloadTexture(value);
    }
    for (auto [name,value] : Sounds) {
        UnloadSound(value);
    }
    //UnloadRenderTexture(GameRenderTexture);
}