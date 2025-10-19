//
// Created by lalit on 8/26/2025.
//

#include "Game.h"

#include <iostream>

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
    CameraShakeIntensity = 0;
    CameraShakeOffset = {0, 0};
    CameraShakeTimer = GetTime();
    Ui = UI(*this);
    MainTileManager = TileManager(*this);
    Entities = std::unordered_map<EntityType, std::vector<shared_ptr<Entity>>>();
    Textures = std::unordered_map<std::string, Texture2D>();
    Weapons = std::unordered_map<std::string, Weapon>();
    PhysicsFPS = 1.0f/240.0f;
    PhysicsAccumulator = 0;
    MainPlayer = nullptr;
    SetGameData();
}

void Game::SetGameData() {
    Textures.insert({"player", LoadTexture("assets/img/player.png")});
    Textures.insert({"enemy", LoadTexture("assets/img/enemy.png")});
    Textures.insert({"bullet", LoadTexture("assets/img/bullet.png")});
    Textures.insert({"sword", LoadTexture("assets/img/sword.png")});
    Textures.insert({"armor_overlay", LoadTexture("assets/img/armor_overlay.png")});
    Sounds.insert({"dash_hit", LoadSound("assets/sounds/dash_hit.wav")});
    Sounds.insert({"death", LoadSound("assets/sounds/death.wav")});
    Sounds.insert({"dash", LoadSound("assets/sounds/dash.mp3")});
    Sounds.insert({"shotgun", LoadSound("assets/sounds/shotgun.wav")});
    Weapons.insert({"Default Gun", {false, false, false, 400, 1.0f, 20, 0.2, 0.0f, 0.0f, 1, 0, "", ""}});
    Weapons.insert({"Player Gun", {false, false, false, 1000, 1.0f, 20, 0.2, 0.0f, 0.0f, 1, 0, "", ""}});
    Weapons.insert({"Shotgun", {false, true, true, 1600, 2.0f, 10, 1.0f, 40.0f, 0.0f, 10, 0.5f, "", "shotgun"}});
    Weapons.insert({"Sword", {true, false, false, 0.4, 1.0f, 35, 0.45f, 90.0f, 350.0f, -1, 0, "sword", ""}});
    Weapons.insert({"Enemy Sword", {true, false, false, 0.4f, 1.0f, 35.0f, 0.55f, 90.0f, 204.0f, -1, 0, "sword", ""}});

    for (int i = 0; i < End; ++i) {
        Entities.insert({(EntityType) i, std::vector<shared_ptr<Entity>>()});
    }
}


void Game::Update() {

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

    while (PhysicsAccumulator >= PhysicsFPS) {
        for (int e = 0; e < End; ++e) {
            std::vector<shared_ptr<Entity>>* array = &Entities[(EntityType)e];
            for (int i = 0; i < array->size(); i++) {
                if (shared_ptr<Entity> entity = array->at(i); entity != nullptr and !entity->ShouldDelete) {
                    entity->PhysicsUpdate(PhysicsFPS);
                }
            }
        }
        PhysicsAccumulator -= PhysicsFPS;
    }

    for (int e = 0; e < End; e++) {
        std::vector<shared_ptr<Entity>> *array = &Entities[(EntityType) e];
        //int old_size = array->size();
        std::erase_if(*array, [this](shared_ptr<Entity> e) {
            if (e && e->ShouldDelete) {
                if (e != MainPlayer) {
                    e.reset();
                }
                return true;
            }
            return false;
        });
        //cout << to_string(e)+"/typa shit ive been on/ de old size(TALLY HALL DETECTED) " + to_string(old_size) + ", new size? (BANANA MAN IS COMING TO NUKE YOU) " + to_string(array->size()) << endl;
    }

    Ui.GameUI();
}

void Game::ShakeCamera(float Intensity) {
    this->CameraShakeIntensity = Intensity;
    this->CameraShakeTimer = GetTime();
    this->CameraShakeOffset = {0, 0};
    this->CameraShakes = 14;
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
    MainTileManager.Map.clear();
    MainPlayer.reset();
}

void Game::Reload(const char *Filename) {

    Clear();

    MainTileManager.ReadMap(Filename);

    MainPlayer = make_shared<Player>((static_cast<float>(MainTileManager.MapWidth) * MainTileManager.TileSize) / 2.0f,
                                     (static_cast<float>(MainTileManager.MapHeight) * MainTileManager.TileSize) / 2.0f, 350.0f,
                                     Textures["player"], *this);
    Entities[PlayerType].push_back(MainPlayer);
}

void Game::Quit() {
    Clear();
    MainTileManager.Quit();
    Ui.Quit();
    for (auto [name,value] : Textures) {
        UnloadTexture(value);
    }
    for (auto [name,value] : Sounds) {
        UnloadSound(value);
    }
}