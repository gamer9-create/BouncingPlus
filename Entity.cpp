//
// Created by lalit on 8/26/2025.
//

#include "Entity.h"
#include "Weapons.h"
#include <ostream>
#include <raylib.h>
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
    this->Health = this->MaxHealth;
    this->Type = DefaultType;
}

Entity::Entity(Texture2D &Texture, Rectangle BoundingBox, float Speed, Game &game) {
    this->game = &game;
    Initialize(Texture, BoundingBox, Speed);
}

Entity::Entity() {

}

Entity::~Entity() {

}

void Entity::PhysicsUpdate(float dt) {
    double dist = std::sqrt((Movement.x * Movement.x) + (Movement.y * Movement.y));
    if (dist != 0) {
        Vector2 FinalMovement = Vector2((Movement.x / dist) * Speed, (Movement.y / dist) * Speed);

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
                    if (tile_id > 0 && tile_id < 3) {
                        float bbox_x = curr_tile_x * game->MainTileManager.TileSize;
                        float bbox_y = curr_tile_y * game->MainTileManager.TileSize;
                        Rectangle bbox = Rectangle(bbox_x, bbox_y, game->MainTileManager.TileSize, game->MainTileManager.TileSize);
                        if (CheckCollisionRecs(BoundingBox, bbox)) {
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
                    if (tile_id > 0 && tile_id < 3) {
                        float bbox_x = curr_tile_x * game->MainTileManager.TileSize;
                        float bbox_y = curr_tile_y * game->MainTileManager.TileSize;
                        Rectangle bbox = Rectangle(bbox_x, bbox_y, game->MainTileManager.TileSize, game->MainTileManager.TileSize);
                        if (CheckCollisionRecs(BoundingBox, bbox)) {
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

void Entity::Update() {
    if (Health <= 0) {
        ShouldDelete = true;
    }
    Vector2 *CameraPosition = &this->game->CameraPosition;
    DrawTexturePro(*Texture, Rectangle(0, 0, static_cast<float> (Texture->width), static_cast<float> (Texture->height)),
                   Rectangle(BoundingBox.x - CameraPosition->x + BoundingBox.width / 2, BoundingBox.y - CameraPosition->y + BoundingBox.height / 2, BoundingBox.width,
                             BoundingBox.height), Vector2(BoundingBox.width / 2, BoundingBox.height / 2), Rotation, WHITE);
}