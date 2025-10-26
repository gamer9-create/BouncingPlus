//
// Created by lalit on 8/29/2025.
//

#ifndef BOUNCINGPLUS_BULLET_H
#define BOUNCINGPLUS_BULLET_H
#include <string>

#include "Entity.h"

using namespace std;

class Bullet : public Entity {
    float ExistenceTimer;
    float Damage;
    bool dd=false;
    Entity& Owner;
    std::string LastBouncedCoordinate;
public:

    bool SlowdownOverTime;
    Vector2 FirePoint;
    Bullet(float X, float Y, float Angle, float Size, float Speed, float Damage, Texture2D& BulletTexture, Entity& Owner, Game &game);
    Bullet();
    void Attack(Entity& entity);
    void PhysicsUpdate(float dt);
    virtual ~Bullet();
    void Update();
};


#endif //BOUNCINGPLUS_BULLET_H