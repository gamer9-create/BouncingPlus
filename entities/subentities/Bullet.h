//
// Created by lalit on 8/29/2025.
//

#ifndef BOUNCINGPLUS_BULLET_H
#define BOUNCINGPLUS_BULLET_H
#include <string>

#include "../Entity.h"

using namespace std;

class Bullet : public Entity {
    float ExistenceTimer;
    float Damage;
    bool DD=false;
    bool RewardedScore=false;

public:
    std::weak_ptr<Entity> OwnerPtr;
    float Lifetime;
    bool SlowdownOverTime;
    float HealthGain;
    Vector2 FirePoint;
    Vector2 LastBouncedCoordinate = {-1, -1};
    Bullet(float X, float Y, float Angle, Vector2 Size, float Speed, float Damage, float Lifetime, Texture2D& BulletTexture, std::shared_ptr<Entity> Owner, Game &game);
    Bullet();
    void Attack(std::shared_ptr<Entity> entity);
    void PhysicsUpdate(float dt, double time);
    virtual ~Bullet();
    void Update();
};


#endif //BOUNCINGPLUS_BULLET_H