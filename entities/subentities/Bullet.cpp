//
// Created by lalit on 8/29/2025.
//

#include "Bullet.h"
#include "typeinfo"
#include <cmath>
#include <ostream>
#include <iostream>
#include <raymath.h>

#include "Enemy.h"
#include "math.h"
#include "../../game/Game.h"

Bullet::Bullet(float X, float Y, float Angle, Vector2 Size, float Speed, float Damage, float Lifetime, Texture2D &BulletTexture, shared_ptr<Entity> Owner, Game &game) : Entity(BulletTexture, BoundingBox, Speed, game) {
    this->Speed = Speed;
    this->Type = BulletType;
    this->ExistenceTimer = 0;
    this->BoundingBox=Rectangle(X - (15 * Size.x / 2.0f), Y - (7.5f * Size.y / 2.0f), 15 * Size.x, 7.5f * Size.y);
    this->Texture=&BulletTexture;
    this->ShouldDelete = false;
    this->SlowdownOverTime = false;
    this->Lifetime=Lifetime;
    this->Speed=Speed;
    this->game = &game;
    this->FirePoint = {X, Y};
    this->Rotation = Angle;//std::atan2(Direction.y, Direction.x) * (180.0f/3.141592653589793238463f);
    this->Damage = Damage;
    this->OwnerPtr = Owner;

    float cX = -cos(Rotation * (2 * PI / 360))*100;
    float cY = -sin(Rotation * (2 * PI / 360))*100;

    this->Movement = {cX, cY};
}

Bullet::Bullet() {

}

Bullet::~Bullet() {

}

void Bullet::PhysicsUpdate(float dt) {

    if (SlowdownOverTime)
        Speed = Lerp(Speed, 0, 10*ExistenceTimer*dt);

    double dist = std::sqrt((Movement.x * Movement.x) + (Movement.y * Movement.y));
    if (dist != 0) {
        Vector2 FinalMovement = Vector2((Movement.x / dist) * Speed, (Movement.y / dist) * Speed);

        bool can_move = true;
        BoundingBox.x += FinalMovement.x * dt;
        BoundingBox.y += FinalMovement.y * dt;
        int tile_x = static_cast<int> (BoundingBox.x / game->MainTileManager.TileSize);
        int tile_y = static_cast<int> (BoundingBox.y / game->MainTileManager.TileSize);
        for (int y = 0; y < 3; y++) {
            for (int x = 0; x < 3; x++) {
                int curr_tile_x = tile_x + x - 1;
                int curr_tile_y = tile_y + y - 1;
                std::string coord = std::to_string(curr_tile_x) + " " + std::to_string(curr_tile_y);
                int tile_id = game->MainTileManager.Map[coord];
                if (game->MainTileManager.TileTypes[tile_id] == WallTileType) {
                    float bbox_x = curr_tile_x * game->MainTileManager.TileSize;
                    float bbox_y = curr_tile_y * game->MainTileManager.TileSize;
                    Rectangle bbox = Rectangle(bbox_x, bbox_y, game->MainTileManager.TileSize, game->MainTileManager.TileSize);
                    if (CheckCollisionRecs(BoundingBox, bbox)) {
                        if (tile_id == 1){// && coord != LastBouncedCoordinate) {
                            int dir_hit = -1; // -1 = none, 0 = left, 1 = up, 2 = right, 3 = down
                            int i= 0;
                            auto dirs = new int[4];

                            Rectangle left_rect = bbox;
                            left_rect.x -= bbox.width;

                            Rectangle right_rect = bbox;
                            right_rect.x += bbox.width;

                            Rectangle up_rect = bbox;
                            up_rect.y -= bbox.height;

                            Rectangle down_rect = bbox;
                            down_rect.y += bbox.height;

                            if (CheckCollisionRecs(BoundingBox, left_rect)) {
                                dir_hit = 0;
                                dirs[i] = dir_hit;
                                i += 1;
                            }

                            if (CheckCollisionRecs(BoundingBox, right_rect)) {
                                dir_hit = 2;
                                dirs[i] = dir_hit;
                                i += 1;
                            }
                            if (CheckCollisionRecs(BoundingBox, up_rect)) {
                                dir_hit = 1;
                                dirs[i] = dir_hit;
                                i += 1;
                            }
                            if (CheckCollisionRecs(BoundingBox, down_rect)) {
                                dir_hit = 3;
                                dirs[i] = dir_hit;
                                i += 1;
                            }

                            bool negate = true;

                            if (i < 2 || dirs[0] == dirs[1]) {
                                if (dir_hit == 1 || dir_hit == 3) {
                                    Rotation = -Rotation;
                                } else {
                                    Rotation = 180 - Rotation;
                                }
                            } else {
                                negate = false;
                            }

                            float X = cos(Rotation * (2 * PI / 360))*100;
                            float Y = sin(Rotation * (2 * PI / 360))*100;

                            if (negate) {
                                Movement = Vector2(-X, -Y);
                            } else {
                                Movement = Vector2(X, Y);
                            }

                            //OwnerPtr.reset();

                            can_move = false;
                        } else if (tile_id == 2) {
                            ShouldDelete = true;
                        }
                    }
                }
            }
        }

        if (!can_move) {
            BoundingBox.x -= FinalMovement.x * dt;
            BoundingBox.y -= FinalMovement.y * dt;
        }
    }
}

void Bullet::Attack(shared_ptr<Entity> entity) {
    if (CheckCollisionCircleRec({BoundingBox.x,BoundingBox.y}, 12, entity->BoundingBox) && !entity->ShouldDelete) {
        bool ShouldDamage = true;
        float ThisDamage = Damage;
        auto Owner = OwnerPtr.lock();
        if (entity->Type == EnemyType) {
            shared_ptr<Enemy> enemy = dynamic_pointer_cast<Enemy>(entity);
            if (Owner != nullptr && Owner->Type == PlayerType)
                enemy->AngeredRangeBypassTimer = enemy->AngeredRangeBypassTimerMax;
            if (Owner != nullptr && Owner->Health > Owner->MaxHealth) {
                float percent = (Owner->Health-Owner->MaxHealth) / Owner->MaxHealth;
                ThisDamage = Damage * (1.0f - min(percent, 0.75f));
            }
            if (enemy->Armor > 0)// && ( (!CloseRangingAllowed && Vector2Distance({BoundingBox.x,BoundingBox.y}, this->FirePoint) > 36) || CloseRangingAllowed ))
            {
                ShouldDamage = false;
                enemy->Armor -= ThisDamage * (1.0f+(GetRandomValue(1, 10)/10.0f));
            }
        }
        if (ShouldDamage)// && ( (!CloseRangingAllowed && Vector2Distance({BoundingBox.x,BoundingBox.y}, this->FirePoint) > 36) || CloseRangingAllowed ))
            entity->Health -= ThisDamage;
        if (Owner != nullptr && entity->Health <= 0 && Owner->Health > 0) {
            Owner->Health += ThisDamage;
            entity->ShouldDelete = true;
            if (Owner->Type == PlayerType)
                game->MainPlayer->Kills += 1;
        }
        ShouldDelete = true;
    }
}

void Bullet::Update() {
    ExistenceTimer += GetFrameTime();
    if (!SlowdownOverTime) {

        if (ExistenceTimer >= Lifetime) {
            ShouldDelete = true;
        }
    } else if (Speed < 50) {
        ShouldDelete = true;
    }
    auto Owner = OwnerPtr.lock();
    for (shared_ptr entity : game->MainEntityManager.Entities[EnemyType]) {
        if (entity != nullptr && entity != Owner && !entity->ShouldDelete) {
            Attack(entity);
        }
    }
    if (Owner == nullptr && !dd) {
        Damage /= 2.0f;
        dd = true;
    }
    if (Owner != game->MainPlayer)
        Attack(game->MainPlayer);
    Entity::Update();
}