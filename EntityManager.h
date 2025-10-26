//
// Created by 7906200 on 10/24/2025.
//

#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H
#include <memory>
#include <unordered_map>
#include <vector>
#include "EntityType.h"

class Game;
class Entity;

class EntityManager {
    void EntityUpdate();
    void EntityPhysicsUpdate();
    void EntityClear();
    float PhysicsAccumulator;
    Game* game;
public:
    std::unordered_map<EntityType, std::vector<Entity>> Entities;
    float PhysicsFPS;
    EntityManager();
    EntityManager(Game &game);
    ~EntityManager();
    void AddEntity(EntityType type, Entity& entity);
    void Update();
    void Reset();
    void Quit();
};



#endif //ENTITYMANAGER_H
