//
// Created by lalit on 8/26/2025.
//

#ifndef BOUNCINGPLUS_ENTITY_H
#define BOUNCINGPLUS_ENTITY_H
#include <complex.h>
#include <raylib.h>

class Game;

enum EntityType {
    EnemyType,
    PlayerType,
    DefaultType
};

class Entity : public std::enable_shared_from_this<Entity> {
    void Initialize(const Texture2D &Texture, Rectangle BoundingBox, float Speed);
    public:
        Game* game;
        Rectangle BoundingBox;
        Texture2D Texture;
        Vector2 Movement;
        EntityType Type;

        float Rotation;
        float Speed;
        float Health;
        float MaxHealth;
        bool ShouldDelete;
        bool CollisionsEnabled;
        Entity();
        virtual ~Entity();

        Entity(const char* Filepath, Rectangle BoundingBox, float Speed, Game &game);
        Entity(Rectangle BoundingBox, float Speed, Game &game);
        Entity(const Texture2D &Texture, Rectangle BoundingBox, float Speed, Game &game);
        virtual void PhysicsUpdate(float dt);
        virtual void Update();
};


#endif //BOUNCINGPLUS_ENTITY_H