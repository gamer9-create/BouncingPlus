//
// Created by lalit on 8/29/2025.
//

#include "Bullet.h"
#include "typeinfo"
#include <cmath>
#include <ostream>
#include <iostream>
#include "math.h"
#include "Game.h"

Bullet::Bullet(float X, float Y, Vector2 Direction, float Speed, float Damage, Texture2D &BulletTexture, std::shared_ptr<Entity> Owner, Game &game) {
    this->Movement = Direction;
    this->Speed = Speed;
    this->ExistenceTimer = 0;
    this->BoundingBox=Rectangle(X - 5, Y - 2.5f, 10, 5);
    this->Texture=BulletTexture;
    this->ShouldDelete = false;
    this->Speed=Speed;
    this->game = &game;
    this->Rotation = std::atan2(Direction.y, Direction.x) * (180.0f/3.141592653589793238463f);
    this->Damage = Damage;
    this->Owner = Owner;
    this->LastBouncedCoordinate = "";
}

Bullet::Bullet() {

}

Bullet::~Bullet() {

}

void Bullet::PhysicsUpdate(float dt) {

    double dist = std::sqrt((Movement.x * Movement.x) + (Movement.y * Movement.y));
    if (dist != 0) {
        Vector2 FinalMovement = Vector2((Movement.x / dist) * Speed, (Movement.y / dist) * Speed);

        BoundingBox.x += FinalMovement.x * dt;
        BoundingBox.y += FinalMovement.y * dt;
        bool should_move = true;
        int tile_x = static_cast<int> (BoundingBox.x / game->MainTileManager.TileSize);
        int tile_y = static_cast<int> (BoundingBox.y / game->MainTileManager.TileSize);
        for (int y = 0; y < 3; y++) {
            for (int x = 0; x < 3; x++) {
                int curr_tile_x = tile_x + x - 1;
                int curr_tile_y = tile_y + y - 1;
                std::string coord = std::to_string(curr_tile_x) + " " + std::to_string(curr_tile_y);
                int tile_id = game->MainTileManager.Map[coord];
                if (tile_id > 0 && tile_id < 3) {
                    float bbox_x = curr_tile_x * game->MainTileManager.TileSize;
                    float bbox_y = curr_tile_y * game->MainTileManager.TileSize;
                    Rectangle bbox = Rectangle(bbox_x, bbox_y, game->MainTileManager.TileSize, game->MainTileManager.TileSize);
                    if (CheckCollisionRecs(BoundingBox, bbox)) {
                        if (tile_id == 1 && coord != LastBouncedCoordinate) {
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

                            if (i < 2 | dirs[0] == dirs[1]) {
                                if (dir_hit == 1 || dir_hit == 3) {
                                    Rotation = -Rotation;
                                } else {
                                    Rotation = 180 - Rotation;
                                }
                            } else {
                                negate = false;
                            }

                            float X = -cos(Rotation * (2 * PI / 360));
                            float Y = -sin(Rotation * (2 * PI / 360));

                            if (negate) {
                                Movement = Vector2(-X, -Y);
                            } else {
                                Movement = Vector2(X, Y);
                            }

                            Owner.reset();

                            //this->LastBouncedCoordinate = coord;
                        } else if (tile_id == 2) {
                            ShouldDelete = true;
                        }
                        should_move = false;
                    }
                }
            }
        }
        if (!should_move) {
            BoundingBox.x -= FinalMovement.x * dt;
            BoundingBox.y -= FinalMovement.y * dt;
        }
    }
}


void Bullet::Update() {
    ExistenceTimer += GetFrameTime();
    if (ExistenceTimer >= 8.5) {
        ShouldDelete = true;
    }
    auto ownerPtr = Owner.lock();
    for (auto& entity : game->Entities) {
        if (entity != nullptr && entity != ownerPtr && !entity->ShouldDelete) {

            if (CheckCollisionRecs(BoundingBox, entity->BoundingBox)) {
                entity->Health -= Damage;
                ShouldDelete = true;
                break;
            }
        }
    }
    Vector2 *CameraPosition = &this->game->CameraPosition;
    DrawTexturePro(Texture, Rectangle(0, 0, static_cast<float> (Texture.width), static_cast<float> (Texture.height)),
                   Rectangle(BoundingBox.x - CameraPosition->x + BoundingBox.width / 2, BoundingBox.y - CameraPosition->y + BoundingBox.height / 2, BoundingBox.width,
                             BoundingBox.height), Vector2(BoundingBox.width / 2, BoundingBox.height / 2), Rotation, WHITE);
}