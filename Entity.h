//
// Created by lalit on 8/26/2025.
//

#ifndef BOUNCINGPLUS_ENTITY_H
#define BOUNCINGPLUS_ENTITY_H
#include <complex.h>
#include <memory>
#include <raylib.h>
#include "EntityType.h"

class Game;

class Entity : public std::enable_shared_from_this<Entity> {
    void Initialize(Texture2D &Texture, Rectangle BoundingBox, float Speed);
    public:
        Game* game;
        Rectangle BoundingBox;
        Texture2D* Texture;
        Vector2 Movement;
        EntityType Type;
        Color EntityColor;

        Vector2 VelocityMovement;
        float VelocityPower;
        std::string LastVelBounceCoord;

        float Rotation;
        float Speed;
        float Health;
        float MaxHealth;
        bool ShouldDelete;
        bool CollisionsEnabled;
        Entity();
        virtual ~Entity();

        Entity(Texture2D &Texture, Rectangle BoundingBox, float Speed, Game &game);
        virtual void PhysicsUpdate(float dt);
        bool IsVisible();
        virtual void Update();
};


#endif //BOUNCINGPLUS_ENTITY_H