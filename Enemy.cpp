//
// Created by lalit on 8/28/2025.
//

#include "iostream"
#include "Enemy.h"

#include <raymath.h>

#include "raylib.h"
#include "Game.h"
#include "Player.h"

#include "UIManager.h"

Enemy::Enemy(float X, float Y, float Health, float Speed, float Armor, std::string Weapon, Texture2D& EnemyTexture, Game &game) : Entity(EnemyTexture,
                                                                   Rectangle(X - 18, Y - 18, 36, 36), Speed, game) {
    this->MaxHealth = Health;
    this->Health = Health;
    this->Speed = Speed;
    this->Armor = Armor;
    this->Type = EnemyType;
    this->AngeredRangeBypassTimer = 0;
    this->AngeredRangeBypassTimerMax = 3;
    this->AnimatedHealth = 0;
    this->MyWeapon = Weapon;
}

Enemy::Enemy() {

}

Enemy::~Enemy() {
}

void Enemy::Update() {
    if (!this->weaponsSystemInit) {
        this->weaponsSystem = WeaponsSystem(shared_from_this(), *game);
        this->weaponsSystem.Weapons[0] = MyWeapon;
        this->weaponsSystem.Equip(0);
        this->weaponsSystemInit = true;
    }
    if (AngeredRangeBypassTimer > 0)
        AngeredRangeBypassTimer -= GetFrameTime();

    if (AngeredRangeBypassTimer <= 0)
        AngeredRangeBypassTimer = 0;

    Movement = Vector2(0, 0);
    float plr_center_x = game->MainPlayer->BoundingBox.x + (game->MainPlayer->BoundingBox.width / 2);
    float plr_center_y = game->MainPlayer->BoundingBox.y + (game->MainPlayer->BoundingBox.height / 2);
    float center_x = BoundingBox.x + (BoundingBox.width / 2);
    float center_y = BoundingBox.y + (BoundingBox.height / 2);
    float distance = std::sqrt(std::pow(plr_center_x - center_x, 2) + std::pow(plr_center_y - center_y, 2));

    if ((distance <= 800 && (distance <= 36 || game->RayCast({center_x, center_y}, {plr_center_x, plr_center_y}))) || AngeredRangeBypassTimer > 0.0f) {
        if (distance >= 100) {
            Movement.x = -(plr_center_x - center_x) / distance * Speed * (weaponsSystem.CurrentWeapon->isMelee ? -1 : 1);
            Movement.y = -(plr_center_y - center_y) / distance * Speed * (weaponsSystem.CurrentWeapon->isMelee ? -1 : 1);
        }
        weaponsSystem.Attack(Vector2(plr_center_x, plr_center_y));
    }

    AnimatedHealth = Lerp(AnimatedHealth, Armor > 0 ? Armor : Health, 10 * GetFrameTime());

    float size = MeasureText(std::to_string((int)round(AnimatedHealth)).c_str(), 36);
    float size2 = MeasureText("%", 18)+1;
    float total_size = size + size2;
    //cout << to_string(GetHealthColor(Health / MaxHealth).r)+" " << to_string(GetHealthColor(Health / MaxHealth).g)+" " << to_string(GetHealthColor(Health / MaxHealth).b)+" " << to_string(GetHealthColor(Health / MaxHealth).a) << endl;

    if ((center_x - total_size / 2 - game->MainCameraManager.CameraPosition.x) > -total_size &&
            (center_x - total_size / 2 - game->MainCameraManager.CameraPosition.x) < GetScreenWidth() &&
            (BoundingBox.y - 36 - game->MainCameraManager.CameraPosition.y) > -36 &&
            (BoundingBox.y - 36 - game->MainCameraManager.CameraPosition.y) < GetScreenHeight()
             )
    {
        std::string t = std::to_string((int) round(AnimatedHealth));
        if (game->DebugDraw) {
            if (t == "67")
                DrawTexture(game->Textures["six_seven"], BoundingBox.x - game->MainCameraManager.CameraPosition.x, BoundingBox.y - game->MainCameraManager.CameraPosition.y, WHITE);
            if (t == "41")
                DrawTexture(game->Textures["fourty_one"], BoundingBox.x - game->MainCameraManager.CameraPosition.x, BoundingBox.y - game->MainCameraManager.CameraPosition.y, WHITE);
        }
        DrawText(t.c_str(),
                 center_x - total_size / 2 - game->MainCameraManager.CameraPosition.x,
                 BoundingBox.y - 36 - game->MainCameraManager.CameraPosition.y, 36, GetHealthColor((Armor > 0 ? Armor : AnimatedHealth) / MaxHealth, Armor));
        DrawText("%", center_x - total_size/2 - game->MainCameraManager.CameraPosition.x + size+1, BoundingBox.y - 22 - game->MainCameraManager.CameraPosition.y, 18, GetHealthColor((Armor > 0 ? Armor : AnimatedHealth) / MaxHealth, Armor));
    }
    weaponsSystem.Update();
    Entity::Update();
    if (IsVisible() && Armor > 0)
        DrawTexturePro(game->Textures["armor_overlay"], {0, 0, BoundingBox.width, BoundingBox.height}, {BoundingBox.x - game->MainCameraManager.CameraPosition.x, BoundingBox.y - game->MainCameraManager.CameraPosition.y, BoundingBox.width, BoundingBox.height}, {0, 0}, 0, WHITE);
}
