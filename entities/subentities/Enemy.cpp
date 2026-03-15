//
// Created by lalit on 8/28/2025.
//

#include "iostream"
#include "Enemy.h"

#include <raymath.h>

#include "raylib.h"

#include "Player.h"
#include "behaviors/WeaponBehavior.h"
#include "../../game/ui/UIManager.h"
#include "../../game/Game.h"

Enemy::Enemy(float X, float Y, float Health, float Speed, float Armor, std::string Weapon, Texture2D& EnemyTexture, Game &game) : Entity(EnemyTexture,Rectangle(X - 18, Y - 18, 36, 36), Speed, game) {
    Init(X,Y,Health,Speed,Armor,Weapon,make_unique<WeaponBehavior>(*this, game),EnemyTexture,game);
}

Enemy::Enemy(float X, float Y, float Health, float Speed, float Armor, std::string Weapon, std::unique_ptr<EnemyBehavior> EnemyBehavior, Texture2D& EnemyTexture, Game& game) : Entity(EnemyTexture,Rectangle(X - 18, Y - 18, 36, 36), Speed, game)
{
    Init(X,Y,Health,Speed,Armor,Weapon,std::move(EnemyBehavior),EnemyTexture,game);
}

Enemy::Enemy() {

}

Enemy::~Enemy() {
}

void Enemy::Init(float X, float Y, float Health, float Speed, float Armor, std::string Weapon,
    std::unique_ptr<EnemyBehavior> behavior, Texture2D& EnemyTexture, Game& game)
{
    this->MaxHealth = Health;
    this->Health = Health;
    this->Speed = Speed;
    this->Armor = Armor;
    this->Type = EnemyType;
    this->AngeredRangeBypassTimer = 0;
    this->AngeredRangeBypassTimerMax = 3;
    this->HealthRegenRate = 0;
    this->AnimatedHealth = 0;
    this->MyWeapon = Weapon;
    this->WanderPos = {BoundingBox.x, BoundingBox.y};
    this->WanderingEnabled = true;
    this->Alpha = 0;
    WanderingCooldown=GetRandomValue(1,4);
    LastSetWanderPos = WanderingCooldown;
    this->EntityColor = ColorAlpha(WHITE, Alpha);
    this->ActivationTimer = game.GetGameTime();
    this->WallMovement = {0, 0};
    this->Behavior = std::move(behavior);
    this->Behavior->Owner = this;
    this->Behavior->game = &game;
}

void Enemy::Wander() {
    if (game->GetGameTime() - LastSetWanderPos >= WanderingCooldown) {
        float center_x = BoundingBox.x + (BoundingBox.width / 2);
        float center_y = BoundingBox.y + (BoundingBox.height / 2);

        Vector2 BestPos = {-1, -1};
        bool S = false;
        bool FoundBest = false;

        for (int i = 0; i < 10; i++)
        {
            float Angle = i * 36.0f;
            Angle += GetRandomValue(-30, 30);
            float X = cos(Angle * (2 * PI / 360))*900;
            float Y = sin(Angle * (2 * PI / 360))*900;
            std::pair<bool, Vector2> d = game->RayCastPoint({center_x,center_y}, {center_x + X,center_y + Y});
            if (!S)
            {
                BestPos = d.second;
                S = true;
            }
            if (Vector2Distance(d.second, {center_x,center_y}) >= Vector2Distance(BestPos, {center_x,center_y}))
            {
                BestPos = d.second;
                FoundBest = true;
            }
        }
        if (FoundBest)
        {
            WanderPos = BestPos;
            LastSetWanderPos = game->GetGameTime();
        } else
        {
            WanderPos = {BoundingBox.x + GetRandomValue(-1000, 1000), BoundingBox.y + GetRandomValue(-1000, 1000)};
        }
    } else if (Vector2Distance({BoundingBox.x, BoundingBox.y}, WanderPos) >= 36) {
        Movement = Vector2Subtract(WanderPos, {BoundingBox.x, BoundingBox.y});
    }
}

void Enemy::OnDelete() {
    if (this->Behavior != nullptr)
        this->Behavior.reset();
    Entity::OnDelete();
}

void Enemy::OnDeath()
{
    game->MainParticleManager.ParticleEffect({
                {BoundingBox.x + BoundingBox.width/2, BoundingBox.y + BoundingBox.height/2},
                300,
                WHITE,
                700,
                6,
                1.75f,
                {255, 0, 0, 255}
            }, Rotation - 180, 360, 15);
    if (GetRandomValue(1, 100) <= 25 && weaponsSystem.CurrentWeaponIndex >= 0 && weaponsSystem.CurrentWeaponIndex <= 2 && !MyWeapon.empty())
    {
        game->PlaceWeaponPickup({
            {BoundingBox.x - BoundingBox.width/2, BoundingBox.y - BoundingBox.height/2},
            RED,
            40,
            -1,
            weaponsSystem.Weapons[weaponsSystem.CurrentWeaponIndex],
            4,
            15
            });
    }
}

void Enemy::Update() {
    if (TotalHealth == -1)
        TotalHealth = Health + Armor;
    RemainingHealthOfOriginalHealth = 0;
    if (Armor <= 0)
        RemainingHealthOfOriginalHealth = Health;
    else
        RemainingHealthOfOriginalHealth = Health + Armor;
    RemainingHealthOfOriginalHealth = RemainingHealthOfOriginalHealth / TotalHealth;

    if (Alpha < 0.9f)
    {
        Alpha = Lerp(Alpha, 1.0f, 2 * game->GetGameDeltaTime());
        EntityColor = ColorAlpha(WHITE, Alpha);
    } else
    {
        Alpha = 1.0f;
    }
    if (ActivationTimer != -1)
        ActivationTimer += game->GetGameDeltaTime();
    if (ActivationTimer > 1) {
        isActive = true;
        ActivationTimer = -1;
    }

    if (!this->weaponsSystemInit) {
        this->weaponsSystem = WeaponsSystem(shared_from_this(), *game);
        this->weaponsSystem.Weapons[0] = MyWeapon;
        this->weaponsSystem.Equip(0);
        this->weaponsSystemInit = true;
    }

    float center_x = BoundingBox.x + (BoundingBox.width / 2);

    if (isActive && Behavior != nullptr)
    {
        bool IsTouchingFreezeZone = false;

        for (std::pair rec : game->FreezeZones)
        {
            if (CheckCollisionRecs(BoundingBox, rec.first))
            {
                IsTouchingFreezeZone = true;
            }
        }

        if (!IsTouchingFreezeZone)
        {
            Behavior->Update();
        } else
        {
            Movement = {0, 0};
        }
    }
    AnimatedHealth = Lerp(AnimatedHealth, Armor > 0 ? Armor : Health, 10 * game->GetGameDeltaTime());

    float size = MeasureText(std::to_string((int)round(AnimatedHealth)).c_str(), 36);
    float size2 = MeasureText("%", 18) + 1;
    float total_size = size + size2;

    if ((center_x - total_size / 2 - game->MainCameraManager.CameraPosition.x) > -total_size &&
        (center_x - total_size / 2 - game->MainCameraManager.CameraPosition.x) < GetScreenWidth() &&
        (BoundingBox.y - 36 - game->MainCameraManager.CameraPosition.y) > -36 &&
        (BoundingBox.y - 36 - game->MainCameraManager.CameraPosition.y) < GetScreenHeight()
    )
    {
        std::string t = std::to_string((int)round(AnimatedHealth));
        if (t == "67") // getting rid of "6 7" meme
            t = "66";
        if (t == "13") // FRIDAY THE 13TH
            t = "12";
        else if (t == "41") // getting rid of 41 meme
            t = "40";
        else if (t == "69") // getting rid of 69 nice number
            t = "68";
        else if (t == "63") // getting rid of rule 63
            t = "62";
        else if (t == "21") // 9 + 10 = ?
            t = "20";
        else if (t == "29") // i blame an old recording of me saying "TWENTY NINE" a year before the "6 7" meme for this
            t = "28";
        else if (t == "34") // stop gooning. just stop. please.
            t = "35";
        else if (t == "131") // idk what this is...
            t = "132";
        else if (t == "420")// i dont think an enemy can even get to this health level... welp i dont like gooners so its gotta go
            t = "419";
        else if (t == "666") // gotta make sure this number doesnt come touchin my game
            t = "665";

        DrawText(t.c_str(),
                 center_x - total_size / 2,
                 BoundingBox.y - 36, 36, GetHealthColor((Armor > 0 ? Armor : AnimatedHealth) / MaxHealth, Armor));
        DrawText("%", center_x - total_size / 2 + size + 1, BoundingBox.y - 22, 18,
                 GetHealthColor((Armor > 0 ? Armor : AnimatedHealth) / MaxHealth, Armor));
    }
    weaponsSystem.Update();
    Entity::Update();
    if (IsVisible() && Armor > 0)
        DrawTexturePro(game->MainResourceManager.Textures["armor_overlay"], {0, 0, BoundingBox.width, BoundingBox.height}, {BoundingBox.x + BoundingBox.width/2, BoundingBox.y + BoundingBox.height/2, BoundingBox.width, BoundingBox.height}, Vector2(BoundingBox.width/2,BoundingBox.height/2), Rotation, EntityColor);
}

void Enemy::MoveAwayFromWalls()
{

    bool nothing_found = true;

    std::vector<Vector2> vectors;

    float center_x = BoundingBox.x + (BoundingBox.width / 2);
    float center_y = BoundingBox.y + (BoundingBox.height / 2);
    int tile_x = static_cast<int> (BoundingBox.x / game->MainTileManager.TileSize);
    int tile_y = static_cast<int> (BoundingBox.y / game->MainTileManager.TileSize);
    for (int y = 0; y < 3; y++)
    {
        for (int x = 0; x < 3; x++)
        {
            int curr_tile_x = tile_x + x - 1;
            int curr_tile_y = tile_y + y - 1;
            std::string coord = std::to_string(curr_tile_x) + " " + std::to_string(curr_tile_y);
            int tile_id = game->MainTileManager.Map[coord];

            float bbox_x = curr_tile_x * game->MainTileManager.TileSize;
            float bbox_y = curr_tile_y * game->MainTileManager.TileSize;

            float plr_center_x = bbox_x + (game->MainTileManager.TileSize / 2);
            float plr_center_y = bbox_y + (game->MainTileManager.TileSize / 2);

            float distance = std::sqrt(std::pow(plr_center_x - center_x, 2) + std::pow(plr_center_y - center_y, 2));
            if (game->MainTileManager.TileTypes[tile_id] == WallTileType)
            {
                nothing_found = false;
                Vector2 d={0,0};
                d.x = -(plr_center_x - center_x) / distance * Speed;
                d.y = -(plr_center_y - center_y) / distance * Speed;
                vectors.push_back(d);
            }
        }
    }

    for (Vector2& vec : vectors)
    {
        WallMovement = Vector2Lerp(WallMovement, vec, 1.0f / vectors.size());
    }

    if (!nothing_found)
        Movement = Vector2Add(WallMovement, Movement);
}
