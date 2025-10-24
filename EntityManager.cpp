//
// Created by 7906200 on 10/24/2025.
//

#include "EntityManager.h"
#include <memory>
#include <unordered_map>
#include <vector>
#include "Game.h"
#include "Entity.h"

EntityManager::EntityManager()
{
}

EntityManager::EntityManager(Game& game)
{
    this->game = &game;
    PhysicsFPS = 240.0f;
    PhysicsAccumulator = 0;
    Entities = std::unordered_map<EntityType, std::vector<shared_ptr<Entity>>>();
    for (int i = 0; i < End; ++i) {
        Entities.insert({(EntityType) i, std::vector<shared_ptr<Entity>>()});
    }
}

EntityManager::~EntityManager()
{
}

void EntityManager::AddEntity(EntityType type, std::shared_ptr<Entity> entity)
{
    Entities[type].push_back(entity);
}

void EntityManager::EntityUpdate()
{
    for (int e = 0; e < End; ++e) {
        std::vector<shared_ptr<Entity>> array = Entities[(EntityType)e];
        for (int i = 0; i < array.size(); i++) {
            if (shared_ptr<Entity> entity = array.at(i); entity != nullptr and !entity->ShouldDelete) {
                entity->Update();
            }
        }
    }
}

void EntityManager::EntityPhysicsUpdate()
{
    PhysicsAccumulator += GetFrameTime();

    while (PhysicsAccumulator >= 1.0f/(PhysicsFPS*game->GameSpeed)) {
        for (int e = 0; e < End; ++e) {
            std::vector<shared_ptr<Entity>> array = Entities[(EntityType)e];
            for (int i = 0; i < array.size(); i++) {
                if (shared_ptr<Entity> entity = array.at(i); entity != nullptr and !entity->ShouldDelete) {
                    entity->PhysicsUpdate(1.0f/PhysicsFPS);
                }
            }
        }
        PhysicsAccumulator -= 1.0f/(PhysicsFPS*game->GameSpeed);
    }
}

void EntityManager::EntityClear()
{
    for (int e = 0; e < End; e++) {
        std::vector<shared_ptr<Entity>> *array = &Entities[(EntityType) e];
        int old_size = array->size();
        std::erase_if(*array, [this](shared_ptr<Entity> e) {
            if (e && e->ShouldDelete) {
                if (e != game->MainPlayer) {
                    e.reset();
                }
                return true;
            }
            return false;
        });
        if (game->DebugDraw) {
            std::string f = to_string(e)+"/typa shit ive been on/ de old size(TALLY HALL DETECTED) " + to_string(old_size) + ", new size? (BANANA MAN IS COMING TO NUKE YOU) " + to_string(array->size());
            DrawText(f.c_str(), 500, 500 + 10*e, 10, WHITE);
        }
    }
}

void EntityManager::Update()
{
    EntityUpdate();
    EntityPhysicsUpdate();
    EntityClear();
}

void EntityManager::Reset()
{
    for (int e = 0; e < End; e++) {
        std::vector<shared_ptr<Entity>>* array = &Entities[(EntityType)e];
        for (int i = 0; i < array->size(); i++) {
            if (shared_ptr<Entity> entity = array->at(i); entity != nullptr) {
                entity.reset();
            }
        }
        array->clear();
    }
}

void EntityManager::Quit()
{
}
