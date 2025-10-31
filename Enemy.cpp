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
    this->WanderPos = {BoundingBox.x, BoundingBox.y};
    this->WanderingEnabled = false;
    this->EntityColor = ColorAlpha(WHITE, 0);
}

Enemy::Enemy() {

}

Enemy::~Enemy() {
}

void Enemy::Wander() {
    if (Vector2Distance({BoundingBox.x, BoundingBox.y}, WanderPos) <= 18) {
        int tile_x = static_cast<int> (BoundingBox.x / game->MainTileManager.TileSize);
        int tile_y = static_cast<int> (BoundingBox.y / game->MainTileManager.TileSize);
        std::vector<Vector2> coords;
        for (int y = 0; y < 5; y++) {
            for (int x = 0; x < 5; x++) {
                int curr_tile_x = tile_x + x - 2;
                int curr_tile_y = tile_y + y - 2;
                std::string coord = std::to_string(curr_tile_x) + " " + std::to_string(curr_tile_y);
                int tile_id = game->MainTileManager.Map[coord];
                if (game->MainTileManager.TileTypes[tile_id] == NothingTileType) {
                    coords.push_back({curr_tile_x * game->MainTileManager.TileSize, curr_tile_y * game->MainTileManager.TileSize});
                }
            }
        }

        if (coords.size() > 0) {
            WanderPos = coords[GetRandomValue(0, coords.size() - 1)];
        }
    } else {
        Movement = Vector2Subtract(WanderPos, {BoundingBox.x, BoundingBox.y});
    }
}

void Enemy::Update() {
    EntityColor = ColorLerp(EntityColor, WHITE, 2 * GetFrameTime());
    if (EntityColor.a > 200)
        isActive = true;
    if (isActive) {
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
        if (weaponsSystem.CurrentWeapon != nullptr && weaponsSystem.TimeStartedReloading == -1 && weaponsSystem.CurrentWeapon->Ammo > 0 && weaponsSystem.WeaponAmmo[weaponsSystem.CurrentWeaponIndex] <= 0)
            weaponsSystem.Reload();

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
        } else if (WanderingEnabled) {
            Wander();
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
            if (t == "67") // getting rid of "6 7" meme
                t = "66";
            else if (t == "41") // getting rid of 41 meme
                t = "40";
            else if (t == "69") // getting rid of 69 nice number
                t = "68";
            else if (t == "63") // getting rid of rule 63
                t = "62";
            else if (t == "21") // 9 + 10 = ?
                t = "20";
            else if (t == "34") // stop gooning. just stop. please.
                t = "35";
            else if (t == "131") // idk what that is...
                t = "132";
            DrawText(t.c_str(),
                     center_x - total_size / 2 - game->MainCameraManager.CameraPosition.x,
                     BoundingBox.y - 36 - game->MainCameraManager.CameraPosition.y, 36, GetHealthColor((Armor > 0 ? Armor : AnimatedHealth) / MaxHealth, Armor));
            DrawText("%", center_x - total_size/2 - game->MainCameraManager.CameraPosition.x + size+1, BoundingBox.y - 22 - game->MainCameraManager.CameraPosition.y, 18, GetHealthColor((Armor > 0 ? Armor : AnimatedHealth) / MaxHealth, Armor));
        }
        weaponsSystem.Update();
    }
    Entity::Update();
    if (isActive) {
        if (IsVisible() && Armor > 0)
            DrawTexturePro(game->Textures["armor_overlay"], {0, 0, BoundingBox.width, BoundingBox.height}, {BoundingBox.x - game->MainCameraManager.CameraPosition.x, BoundingBox.y - game->MainCameraManager.CameraPosition.y, BoundingBox.width, BoundingBox.height}, {0, 0}, 0, WHITE);
    }
}
