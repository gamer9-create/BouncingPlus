//
// Created by lalit on 8/28/2025.
//

#include "iostream"
#include "Enemy.h"
#include "raylib.h"
#include "Game.h"
#include "Player.h"

using namespace std;

Enemy::Enemy(float X, float Y, int AILevel, float Health, float Speed, Texture2D& EnemyTexture, Game &game) : Entity(EnemyTexture,
                                                                   Rectangle(X - 18, Y - 18, 36, 36), Speed, game) {
    this->MaxHealth = Health;
    this->Health = Health;
    this->AILevel = AILevel;
    this->Speed = Speed;
    this->Type = EnemyType;
}

Enemy::Enemy() {

}

Enemy::~Enemy() {

}


void Enemy::Update() {
    if (!weaponsSystemInit) {
        this->weaponsSystem = WeaponsSystem(shared_from_this(), *game);
        this->weaponsSystem.Weapons[0] = "Default Gun";
        this->weaponsSystem.Equip(0);
        weaponsSystemInit = true;
    }
    Movement = Vector2(0, 0);
    std::shared_ptr<Player> MainPlayer = this->game->MainPlayer;
    float plr_center_x = MainPlayer->BoundingBox.x + (MainPlayer->BoundingBox.width / 2);
    float plr_center_y = MainPlayer->BoundingBox.y + (MainPlayer->BoundingBox.height / 2);
    float center_x = BoundingBox.x + (BoundingBox.width / 2);
    float center_y = BoundingBox.y + (BoundingBox.height / 2);
    float distance = std::sqrt(std::pow(plr_center_x - center_x, 2) + std::pow(plr_center_y - center_y, 2));
    if (distance <= 250) {
        Movement.x = -(plr_center_x - center_x) / distance * Speed;
        Movement.y = -(plr_center_y - center_y) / distance * Speed;
        weaponsSystem.Attack(Vector2(plr_center_x, plr_center_y));
    }
    weaponsSystem.Update();
    Entity::Update();
}

