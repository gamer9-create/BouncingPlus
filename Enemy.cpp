//
// Created by lalit on 8/28/2025.
//

#include "iostream"
#include "Enemy.h"

#include <raymath.h>

#include "raylib.h"
#include "Game.h"
#include "Player.h"

#include "UI.h"

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

bool Enemy::Raycast(float target_x, float target_y) {
    float ray_x = BoundingBox.x;
    float ray_y = BoundingBox.y;
    auto start_distance = static_cast<float>(sqrt(pow(ray_x - target_x, 2) + pow(ray_y - target_y, 2)));

    const float step_x = ((target_x-ray_x) / start_distance) * min(start_distance/10.0f, 18.0f);
    const float step_y = ((target_y-ray_y) / start_distance) * min(start_distance/10.0f, 18.0f);

    bool x_side = (target_x - ray_x) < 0;
    bool y_side = (target_y - ray_y) < 0;

    while (sqrt(pow(ray_x - target_x, 2) + pow(ray_y - target_y, 2)) > 10 && !CheckCollisionPointRec({ray_x,ray_y},game->MainPlayer->BoundingBox) && !(target_x == ray_x && target_y == ray_y)) {
        //DrawCircle(ray_x-game->CameraPosition.x, ray_y-game->CameraPosition.y, 5, ColorAlpha(WHITE, start_distance / 500.0f));
        std::string coord = std::to_string((int) (ray_x / game->MainTileManager.TileSize)) + " " + std::to_string((int) (ray_y / game->MainTileManager.TileSize));
        if (int tile_id = game->MainTileManager.Map[coord]; tile_id > 0 && tile_id <= 2) {
            return false;
        }
        bool curr_x_side = (target_x - ray_x) < 0;
        bool curr_y_side = (target_y - ray_y) < 0;
        if ((curr_x_side != x_side || curr_y_side != y_side) && (target_x != ray_x || target_y != ray_y)) {
            return false;
        }
        ray_x += step_x;
        ray_y += step_y;
    }
    return true;
}

void Enemy::Update() {
    if (!this->weaponsSystemInit) {
        this->weaponsSystem = WeaponsSystem(shared_from_this(), *game);
        this->weaponsSystem.Weapons[0] = MyWeapon;
        this->weaponsSystem.Equip(0);
        this->weaponsSystemInit = true;
    }
    if (AngeredRangeBypassTimer > 0){
        AngeredRangeBypassTimer -= GetFrameTime();
    }
    if (AngeredRangeBypassTimer <= 0)
        AngeredRangeBypassTimer = 0;
    Movement = Vector2(0, 0);
    float plr_center_x = game->MainPlayer->BoundingBox.x + (game->MainPlayer->BoundingBox.width / 2);
    float plr_center_y = game->MainPlayer->BoundingBox.y + (game->MainPlayer->BoundingBox.height / 2);
    float center_x = BoundingBox.x + (BoundingBox.width / 2);
    float center_y = BoundingBox.y + (BoundingBox.height / 2);
    float distance = std::sqrt(std::pow(plr_center_x - center_x, 2) + std::pow(plr_center_y - center_y, 2));
    if ((distance <= 800 && (distance <= 36 || Raycast(game->MainPlayer->BoundingBox.x, game->MainPlayer->BoundingBox.y))) || AngeredRangeBypassTimer > 0.0f) {
        Movement.x = -(plr_center_x - center_x) / distance * Speed * (weaponsSystem.CurrentWeapon->isMelee ? -1 : 1);
        Movement.y = -(plr_center_y - center_y) / distance * Speed * (weaponsSystem.CurrentWeapon->isMelee ? -1 : 1);
        weaponsSystem.Attack(Vector2(plr_center_x, plr_center_y));
    }

    AnimatedHealth = Lerp(AnimatedHealth, Health, 10 * GetFrameTime());

    float size = MeasureText(std::to_string((int)round(this->AnimatedHealth)).c_str(), 36);
    float size2 = MeasureText("%", 18)+1;
    float total_size = size + size2;
    //cout << to_string(GetHealthColor(Health / MaxHealth).r)+" " << to_string(GetHealthColor(Health / MaxHealth).g)+" " << to_string(GetHealthColor(Health / MaxHealth).b)+" " << to_string(GetHealthColor(Health / MaxHealth).a) << endl;
    if ((BoundingBox.x + BoundingBox.width / 2 - total_size / 2 - game->CameraPosition.x) > -total_size &&
            (BoundingBox.x + BoundingBox.width / 2 - total_size / 2 - game->CameraPosition.x) < GetScreenWidth() &&
            (BoundingBox.y - 36 - game->CameraPosition.y) > -36 &&
            (BoundingBox.y - 36 - game->CameraPosition.y) < GetScreenHeight()
            )
    {
        DrawText(std::to_string((int) round(this->AnimatedHealth)).c_str(),
                 BoundingBox.x + BoundingBox.width / 2 - total_size / 2 - game->CameraPosition.x,
                 BoundingBox.y - 36 - game->CameraPosition.y, 36, GetHealthColor(AnimatedHealth / MaxHealth, Armor));
        DrawText("%", BoundingBox.x + BoundingBox.width/2 - total_size/2 - game->CameraPosition.x + size+1, BoundingBox.y - 22 - game->CameraPosition.y, 18, GetHealthColor(AnimatedHealth / MaxHealth, Armor));
    }
    weaponsSystem.Update();
    Entity::Update();

}