//
// Created by lalit on 8/27/2025.
//

#include "Player.h"

#include <iostream>
#include <ostream>

#include "Game.h"

using namespace std;

Player::Player(float X, float Y, float Speed, Texture2D &PlayerTexture, Game &game) : Entity(PlayerTexture,
                                                                   Rectangle(X - 18, Y - 18, 36, 36), Speed, game) {
    this->Type = PlayerType;
    this->Kills = 0;
    this->OrigSpeed = Speed;

}

Player::Player() {
}

Player::~Player() {

}

void Player::PhysicsUpdate(float dt) {
    float MovementX = 0;
    float MovementY = 0;
    if (IsKeyDown(KEY_W)) {
        MovementY -= Speed;
    }
    if (IsKeyDown(KEY_S)) {
        MovementY += Speed;
    }
    if (IsKeyDown(KEY_A)) {
        MovementX -= Speed;
    }
    if (IsKeyDown(KEY_D)) {
        MovementX += Speed;
    }
    Movement = Vector2(MovementX, MovementY);
    Entity::PhysicsUpdate(dt);
}

void Player::Update() {
    if (!this->weaponsSystemInit) {
        this->weaponsSystem = WeaponsSystem(shared_from_this(), *game);
        this->weaponsSystem.Weapons[0] = "Player Gun";
        this->weaponsSystem.Weapons[1] = "Sniper Gun";
        this->weaponsSystem.Weapons[2] = "Sword";
        this->weaponsSystem.Equip(0);
        this->weaponsSystemInit = true;
    }
    if (IsMouseButtonDown(0)) {
        weaponsSystem.Attack(Vector2(static_cast<float> (GetMouseX()) + game->CameraPosition.x, static_cast<float> (GetMouseY()) + game->CameraPosition.y));
    }
    if (IsKeyPressed(KEY_ONE)) {
        if (weaponsSystem.CurrentWeaponIndex != 0) {
            weaponsSystem.Equip(0);
        } else if (weaponsSystem.CurrentWeaponIndex == 0) {
            weaponsSystem.Unequip();
        }
    }
    if (IsKeyPressed(KEY_TWO)) {
        if (weaponsSystem.CurrentWeaponIndex != 1) {
            weaponsSystem.Equip(1);
        } else if (weaponsSystem.CurrentWeaponIndex == 1) {
            weaponsSystem.Unequip();
        }
    }
    if (IsKeyPressed(KEY_THREE)) {
        if (weaponsSystem.CurrentWeaponIndex != 2) {
            weaponsSystem.Equip(2);
        } else if (weaponsSystem.CurrentWeaponIndex == 2) {
            weaponsSystem.Unequip();
        }
    }
    if ((Health/MaxHealth) > 2.0f)
    {
        Speed = OrigSpeed * (1.0f - min((Health/MaxHealth)-2.0f, 0.4f));
    } else
    {
        Speed = OrigSpeed;
    }
    Entity::Update();
    weaponsSystem.Update();
}