//
// Created by lalit on 8/26/2025.
//

#include "Entity.h"
#include "Weapons.h"
#include <ostream>
#include <raylib.h>
#include <raymath.h>

#include "Bullet.h"
#include "Game.h"

void Entity::Initialize(Texture2D &Texture, Rectangle BoundingBox, float Speed) {
    this->Texture = &Texture;
    this->BoundingBox = BoundingBox;
    this->Movement = Vector2(0, 0);
    this->Speed = Speed;
    this->Rotation = 0;
    this->ShouldDelete = false;
    this->CollisionsEnabled = true;
    this->MaxHealth = 100;
    this->WeaponWeightSpeedMultiplier = 1;
    this->Health = this->MaxHealth;
    this->Type = DefaultType;
    this->VelocityMovement = {0,0};
    this->VelocityPower = 0;
    this->LastVelBounceCoord = "";
    this->EntityColor = WHITE;
}

Entity::Entity(Texture2D &Texture, Rectangle BoundingBox, float Speed, Game &game) {
    this->game = &game;
    Initialize(Texture, BoundingBox, Speed);
}

Entity::Entity() {

}

Entity::~Entity() {

}

float Entity::GetSpeed() {
    return Speed * WeaponWeightSpeedMultiplier;
}

void Entity::PhysicsUpdate(float dt) {
    if (abs(VelocityPower) > 0) {
        VelocityPower += 1000.0f * dt * (VelocityPower > 0 ? -1 : 1);
        if (abs(VelocityPower) < 5)
            VelocityPower = 0;
    }
    Vector2 mov = Vector2Normalize(Movement);
    Vector2 vel = Vector2Normalize(VelocityMovement);
    Vector2 FinalMovement = Vector2((mov.x * GetSpeed()) + (vel.x * VelocityPower), (mov.y * GetSpeed()) + (vel.y * VelocityPower));
    if (Vector2Distance({0,0}, vel) != 0 && CollisionsEnabled) {
        BoundingBox.x += FinalMovement.x * dt;
        BoundingBox.y += FinalMovement.y * dt;
        bool f = false;
        int tile_x = static_cast<int> (BoundingBox.x / game->MainTileManager.TileSize);
        int tile_y = static_cast<int> (BoundingBox.y / game->MainTileManager.TileSize);
        for (int y = 0; y < 3; y++) {
            if (f)
                break;
            for (int x = 0; x < 3; x++) {
                int curr_tile_x = tile_x + x - 1;
                int curr_tile_y = tile_y + y - 1;
                std::string coord = std::to_string(curr_tile_x) + " " + std::to_string(curr_tile_y);
                int tile_id = game->MainTileManager.Map[coord];
                float bbox_x = curr_tile_x * game->MainTileManager.TileSize;
                float bbox_y = curr_tile_y * game->MainTileManager.TileSize;
                Rectangle bbox = Rectangle(bbox_x, bbox_y, game->MainTileManager.TileSize, game->MainTileManager.TileSize);
                if (game->MainTileManager.TileTypes[tile_id] == WallTileType && CheckCollisionRecs(BoundingBox, bbox) && coord != LastVelBounceCoord) {

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

                    float eRotation = std::atan2(VelocityMovement.y, VelocityMovement.x) * (180.0f/3.141592653589793238463f);

                    if (i < 2 | dirs[0] == dirs[1]) {
                        if (dir_hit == 1 || dir_hit == 3) {
                            eRotation = -eRotation;
                        } else {
                            eRotation = 180 - eRotation;
                        }
                    } else {
                        negate = false;
                    }

                    float X = -cos(eRotation * (2 * PI / 360));
                    float Y = -sin(eRotation * (2 * PI / 360));

                    if (negate) {
                        VelocityMovement = Vector2(-X, -Y);
                    } else {
                        VelocityMovement = Vector2(X, Y);
                    }

                    VelocityPower /= 1.5f;
                    LastVelBounceCoord = coord;
                    f =true;
                    break;
                }
            }
        }
        BoundingBox.x -= FinalMovement.x * dt;
        BoundingBox.y -= FinalMovement.y * dt;
        FinalMovement = Vector2((mov.x * GetSpeed()) + (vel.x * VelocityPower), (mov.y * GetSpeed()) + (vel.y * VelocityPower));
    }
    if (Vector2Distance({0,0}, FinalMovement) > 0) {
        if (CollisionsEnabled) {

            BoundingBox.x += FinalMovement.x * dt;
            bool can_move_x = true;
            int tile_x = static_cast<int> (BoundingBox.x / game->MainTileManager.TileSize);
            int tile_y = static_cast<int> (BoundingBox.y / game->MainTileManager.TileSize);
            for (int y = 0; y < 3; y++) {
                for (int x = 0; x < 3; x++) {
                    int curr_tile_x = tile_x + x - 1;
                    int curr_tile_y = tile_y + y - 1;
                    std::string coord = std::to_string(curr_tile_x) + " " + std::to_string(curr_tile_y);
                    int tile_id = game->MainTileManager.Map[coord];
                    float bbox_x = curr_tile_x * game->MainTileManager.TileSize;
                    float bbox_y = curr_tile_y * game->MainTileManager.TileSize;
                    if (game->DebugDraw) {
                        DrawRectangleRec(
                            {
                                bbox_x - game->MainCameraManager.CameraPosition.x,
                                bbox_y - game->MainCameraManager.CameraPosition.y,
                                game->MainTileManager.TileSize,
                                game->MainTileManager.TileSize,
                            }, ColorAlpha(GREEN, 0.2f));
                    }
                    if (game->MainTileManager.TileTypes[tile_id] == WallTileType) {
                        Rectangle bbox = Rectangle(bbox_x, bbox_y, game->MainTileManager.TileSize, game->MainTileManager.TileSize);
                        if (CheckCollisionRecs(BoundingBox, bbox)) {
                            //float e_cx = BoundingBox.x + (BoundingBox.width / 2.0f);
                            //float t_cx = bbox_x + (game->MainTileManager.TileSize / 2.0f);
                            //normal_x = (t_cx > e_cx ? 1 : -1);
                            //normal_x = (t_cx == e_cx ? 0 : normal_x);
                            can_move_x = false;
                            break;
                        }
                    }
                }
                if (!can_move_x) {
                    break;
                }
            }
            if (!can_move_x) {
                BoundingBox.x -= FinalMovement.x * dt;
            }

            BoundingBox.y += FinalMovement.y * dt;
            can_move_x = true;
            tile_x = static_cast<int> (BoundingBox.x / game->MainTileManager.TileSize);
            tile_y = static_cast<int> (BoundingBox.y / game->MainTileManager.TileSize);
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
                            //float e_cy = BoundingBox.y + (BoundingBox.height / 2.0f);
                            //float t_cy = bbox_y + (game->MainTileManager.TileSize / 2.0f);
                            //normal_y = (t_cy > e_cy ? 1 : -1);
                            //normal_y = (t_cy == e_cy ? 0 : normal_y);
                            can_move_x = false;
                            break;
                        }
                    }
                }
                if (!can_move_x) {
                    break;
                }
            }
            if (!can_move_x) {
                BoundingBox.y -= FinalMovement.y * dt;
            }
        } else {
            BoundingBox.x += FinalMovement.x * dt;
            BoundingBox.y += FinalMovement.y * dt;
        }


    }
}

bool Entity::IsVisible() {
    Vector2 *CameraPosition = &this->game->MainCameraManager.CameraPosition;
    return BoundingBox.x - CameraPosition->x > -BoundingBox.width &&
        BoundingBox.x - CameraPosition->x < GetScreenWidth() &&
        BoundingBox.y - CameraPosition->y > -BoundingBox.height &&
        BoundingBox.y - CameraPosition->y < GetScreenHeight();
}

void Entity::Update() {
    if (Health <= 0) {
        ShouldDelete = true;
    }
    Vector2 *CameraPosition = &this->game->MainCameraManager.CameraPosition;

    if (IsVisible() && Texture != nullptr)
    {
        DrawTexturePro(*Texture, Rectangle(0, 0, static_cast<float> (Texture->width), static_cast<float> (Texture->height)),
                       Rectangle(BoundingBox.x - CameraPosition->x + BoundingBox.width/2, BoundingBox.y - CameraPosition->y + BoundingBox.height/2, BoundingBox.width,
                                 BoundingBox.height), Vector2(BoundingBox.width/2,BoundingBox.height/2), Rotation, EntityColor);
    }
}

void Entity::OnDelete() {

}
