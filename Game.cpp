//
// Created by lalit on 8/26/2025.
//

#include "Game.h"
#include <ostream>
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
    Entities = std::vector<std::shared_ptr<Entity>>();
    Textures = std::unordered_map<std::string, Texture2D>();
    Bullets = std::vector<std::shared_ptr<Bullet>>();
    Weapons = std::unordered_map<std::string, Weapon>();
    PhysicsFPS = 1.0f/240.0f;
    PhysicsAccumulator = 0;

    SetGameData();
}

void Game::SetGameData() {
    Textures.insert({"player", LoadTexture("assets/img/player.png")});
    Textures.insert({"enemy", LoadTexture("assets/img/enemy.png")});
    Textures.insert({"bullet", LoadTexture("assets/img/bullet.png")});
    Weapons.insert({"Default Gun", {false, 400, 20, 0.2, -1}});
    Weapons.insert({"Player Gun", {false, 800, 20, 0.2, -1}});
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
    for (int i = 0; i < Entities.size(); i++) {
        if (std::shared_ptr<Entity> entity = Entities[i]; entity != nullptr and !entity->ShouldDelete) {
            entity->Update();
        }
    }
    for (int i = 0; i < Bullets.size(); i++) {
        if (std::shared_ptr<Bullet> bullet = Bullets[i]; bullet != nullptr and !bullet->ShouldDelete) {
            bullet->Update();
        }
    }

    while (PhysicsAccumulator >= PhysicsFPS) {
        for (int i = 0; i < Entities.size(); i++) {
            if (std::shared_ptr<Entity> entity = Entities[i]; entity != nullptr and !entity->ShouldDelete) {
                entity->PhysicsUpdate(PhysicsFPS);
            }
        }
        for (int i = 0; i < Bullets.size(); i++) {
            if (std::shared_ptr<Bullet> bullet = Bullets[i]; bullet != nullptr and !bullet->ShouldDelete) {
                bullet->PhysicsUpdate(PhysicsFPS);
            }
        }
        PhysicsAccumulator -= PhysicsFPS;
    }

    std::erase_if(Entities, [](shared_ptr<Entity>& e) {
        if (e && e->ShouldDelete) {
            return true;
        }
        return false;
    });

    Ui.WeaponUI();
}

void Game::Reload(const char *Filename) {
    Entities.clear();
    MainTileManager.ReadMap(Filename);

    MainPlayer = make_shared<Player>((static_cast<float>(MainTileManager.MapWidth) * MainTileManager.TileSize) / 2.0f,
                                     (static_cast<float>(MainTileManager.MapHeight) * MainTileManager.TileSize) / 2.0f, 250.0f,
                                     Textures["player"], *this);
    Entities.push_back(MainPlayer);
}

void Game::Quit() {
    Entities.clear();
    MainTileManager.Map.clear();
    MainPlayer.reset();
}