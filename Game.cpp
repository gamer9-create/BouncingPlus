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
    CameraSpeed = 20.0f;
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
    Textures.insert({"player_sword", LoadTexture("assets/img/sword.png")});
    Weapons.insert({"Default Gun", {false, 400, 20, 0.2, 0.0f, 0.0f, -1, ""}});
    Weapons.insert({"Player Gun", {false, 800, 20, 0.2, 0.0f, 0.0f, -1, ""}});
    Weapons.insert({"Player Sword", {true, 0.4, 35, 1, 90.0f, 204.0f, -1, "player_sword"}});

    for (int i = 0; i < End; ++i) {
        Entities.insert({(EntityType) i, std::vector<shared_ptr<Entity>>()});
    }
}


void Game::Update() {

    PhysicsAccumulator += GetFrameTime();

    float TargetX = CameraTarget.x - CameraPosition.x - (static_cast<float>(GetScreenWidth()) / 2.0f);
    float TargetY = CameraTarget.y - CameraPosition.y - (static_cast<float>(GetScreenHeight()) / 2.0f);

    float ImportantVal = 20.0f * (static_cast<float>(GetFPS()) / 144.0f);
    if (ImportantVal != 0.0f) {
        CameraPosition.x += TargetX / ImportantVal;
        CameraPosition.y += TargetY / ImportantVal;
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

    Ui.WeaponUI();
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
                                     (static_cast<float>(MainTileManager.MapHeight) * MainTileManager.TileSize) / 2.0f, 250.0f,
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
}