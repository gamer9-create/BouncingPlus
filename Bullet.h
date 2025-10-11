//
// Created by lalit on 8/29/2025.
//

#ifndef BOUNCINGPLUS_BULLET_H
#define BOUNCINGPLUS_BULLET_H
#include <string>

#include "Entity.h"

using namespace std;

class Bullet : public Entity {
public:
    float ExistenceTimer;
    float Damage;
    std::weak_ptr<Entity> OwnerPtr;
    std::string LastBouncedCoordinate;
    Bullet(float X, float Y, Vector2 Direction, float Speed, float Damage, Texture2D& BulletTexture, std::shared_ptr<Entity> Owner, Game &game);
    Bullet();
    void Attack(std::shared_ptr<Entity> entity);
    void PhysicsUpdate(float dt);
    virtual ~Bullet();
    void Update();
};


#endif //BOUNCINGPLUS_BULLET_H