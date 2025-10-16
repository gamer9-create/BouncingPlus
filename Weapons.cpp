//
// Created by lalit on 9/14/2025.
//

#include "raylib.h"
#include "raymath.h"
#include "Entity.h"
#include "Bullet.h"
#include "Game.h"
#include "Weapons.h"
#include "iostream"

using namespace std;

WeaponsSystem::WeaponsSystem(shared_ptr<Entity> Owner, Game& game) {
    AttackAmmo = -1;
    CurrentWeapon = nullptr;
    this->game = &game;
    this->OwnerPtr = Owner;
    AttackCooldown = 0;
    CurrentWeaponIndex = 0;
    MeleeAnim = false;
    MeleeAnimTexture = nullptr;
    MeleeAnimAngle = 0;
    MeleeAnimRange = 45;
    MeleeAnimPercent = 0;
    PointRemovalTimer = 0;
    MeleeAnimAlpha = 1;
}

WeaponsSystem::WeaponsSystem() {
}

WeaponsSystem::~WeaponsSystem() {
}

void WeaponsSystem::Update() {
    auto Owner = OwnerPtr.lock();
    if (CurrentWeapon != nullptr && CurrentWeapon->isMelee && AttackCooldown >= CurrentWeapon->Cooldown) {
        MeleeAnimRange = CurrentWeapon->AngleRange;
        float cx = Owner->BoundingBox.x + Owner->BoundingBox.width / 2;
        float cy = Owner->BoundingBox.y + Owner->BoundingBox.height / 2;
        float Angle = (atan2(cy - (GetMouseY() + game->CameraPosition.y), cx - (GetMouseX() + game->CameraPosition.x)) * RAD2DEG)+180;

        float z = (Angle - MeleeAnimRange/2) * DEG2RAD;
        float dist = CurrentWeapon->Range;
        Vector2 AnglePos1 = {cosf(z)*dist, sinf(z)*dist};
        z = (Angle + MeleeAnimRange/2) * DEG2RAD;
        Vector2 AnglePos2 = {cosf(z)*dist, sinf(z)*dist};

        Vector2 OwnerPos = {cx, cy};
        AnglePos1.x += OwnerPos.x;
        AnglePos1.y += OwnerPos.y;
        AnglePos2.x += OwnerPos.x;
        AnglePos2.y += OwnerPos.y;

        DrawTriangle(AnglePos1 - game->CameraPosition, OwnerPos - game->CameraPosition, AnglePos2 - game->CameraPosition, ColorAlpha(WHITE, 0.5f));
    }

    if (points.size() != 0) {
        for (int i = 0; i < points.size(); i++) {
            if (i != 0) {
                Vector2 f1 = {points.at(i-1).x + Owner->BoundingBox.x + Owner->BoundingBox.width/2 - game->CameraPosition.x, points.at(i-1).y+Owner->BoundingBox.y + Owner->BoundingBox.height/2  - game->CameraPosition.y};
                Vector2 f = {points.at(i).x + Owner->BoundingBox.x + Owner->BoundingBox.width/2 - game->CameraPosition.x, points.at(i).y+ Owner->BoundingBox.y + Owner->BoundingBox.height/2 - game->CameraPosition.y};
                DrawLineBezier(f, f1, 10, WHITE);
            }
        }
    }

    if (GetTime() - PointRemovalTimer >= 0.001 && points.size() > 0) {
        points.erase(points.begin()+points.size()-1);
        PointRemovalTimer = GetTime();
    }

    if (AttackAmmo == -1 && CurrentWeapon != nullptr)
        AttackAmmo = CurrentWeapon->Ammo;
    if (!MeleeAnim) {
        MeleeAnimTexture = nullptr;
        MeleeAnimAngle = 0;
        MeleeAnimRange = 90;
        MeleeAnimPercent = 0;
        MeleeAnimAlpha = 1;
    } else {
        if (CurrentWeapon != nullptr) {
            MeleeAnimRange = CurrentWeapon->AngleRange;
        }
        if (MeleeAnimPercent <= 1.0)
            MeleeAnimPercent += 3 * GetFrameTime();
        MeleeAnim = MeleeAnimPercent <= 1.0 || MeleeAnimAlpha > 0;

        float width = (MeleeAnimTexture->width * 1.35f);
        float height = (MeleeAnimTexture->height * 1.35f);
        float FinalAngle = MeleeAnimAngle - (MeleeAnimRange/2) + (MeleeAnimPercent * MeleeAnimRange);

        if (MeleeAnimPercent <= 1.0) {
            points.clear();
            float length = 30;
            int times = 25;
            for (int i = 0; i < times; i++) {
                for (int x = 5; x < 10; x++) {
                    float ThisAngle = max(MeleeAnimAngle - (MeleeAnimRange/2), FinalAngle - ((length/times) * i));;
                    points.push_back({-cosf((ThisAngle+90) * DEG2RAD)*(20.4f)*x,
                        -sinf((ThisAngle+90) * DEG2RAD)*(20.4f)*x});
                }
            }
        } else {
            MeleeAnimAlpha -= 3 * GetFrameTime();
        }

        DrawTexturePro(*MeleeAnimTexture, Rectangle(0, 0, static_cast<float> (MeleeAnimTexture->width), static_cast<float> (MeleeAnimTexture->height)),
                   Rectangle(Owner->BoundingBox.x + Owner->BoundingBox.width/2 - game->CameraPosition.x - cosf((FinalAngle+90) * DEG2RAD)*150, Owner->BoundingBox.y + Owner->BoundingBox.height/2 - game->CameraPosition.y - sinf((FinalAngle+90) * DEG2RAD)*150, width,
                             height), Vector2(width / 2, height / 2), FinalAngle, ColorAlpha(WHITE, MeleeAnimAlpha));

    }
    AttackCooldown += GetFrameTime();

}

bool WeaponsSystem::Raycast(Rectangle target) {
    auto Owner = OwnerPtr.lock();
    float ray_x = Owner->BoundingBox.x;
    float ray_y = Owner->BoundingBox.y;
    float target_x = target.x + target.width/2.0f;
    float target_y = target.y + target.height/2.0f;
    auto start_distance = static_cast<float>(sqrt(pow(ray_x - target_x, 2) + pow(ray_y - target_y, 2)));

    const float step_x = ((target_x-ray_x) / start_distance) * 18;
    const float step_y = ((target_y-ray_y) / start_distance) * 18;

    bool x_side = (target_x - ray_x) < 0;
    bool y_side = (target_y - ray_y) < 0;

    while (sqrt(pow(ray_x - target_x, 2) + pow(ray_y - target_y, 2)) > 2 && !CheckCollisionPointRec({ray_x,ray_y},target)) {
        ray_x += step_x;
        ray_y += step_y;
        //DrawCircle(ray_x-game->CameraPosition.x, ray_y-game->CameraPosition.y, 1, WHITE);
        std::string coord = std::to_string((int) (ray_x / game->MainTileManager.TileSize)) + " " + std::to_string((int) (ray_y / game->MainTileManager.TileSize));
        if (int tile_id = game->MainTileManager.Map[coord]; tile_id > 0 && tile_id <= 2) {
            return false;
        }
        bool curr_x_side = (target_x - ray_x) < 0;
        bool curr_y_side = (target_y - ray_y) < 0;
        if (curr_x_side != x_side || curr_y_side != y_side) {
            return false;
        }
    }
    return true;
}

void WeaponsSystem::Attack(Vector2 Target) {
    auto Owner = OwnerPtr.lock();
    if (CurrentWeapon != nullptr && AttackCooldown >= CurrentWeapon->Cooldown && (AttackAmmo > 0 || CurrentWeapon->Ammo == -1)) {
        if (!CurrentWeapon->isMelee) {
            Target.x -= Owner->BoundingBox.x + (Owner->BoundingBox.width / 2);
            Target.y -= Owner->BoundingBox.y + (Owner->BoundingBox.height / 2);
            shared_ptr<Bullet> bullet = make_shared<Bullet>(Owner->BoundingBox.x + (Owner->BoundingBox.width / 2), Owner->BoundingBox.y + (Owner->BoundingBox.height / 2), Target, CurrentWeapon->Speed, CurrentWeapon->Damage,
                                                            game->Textures["bullet"], Owner, *game);
            game->Entities[BulletType].push_back(bullet);
            if (CurrentWeapon->Ammo != -1)
                AttackAmmo -= 1;
        } else {
            float Angle = atan2(Owner->BoundingBox.y - Target.y, Owner->BoundingBox.x - Target.x) * RAD2DEG;
            this->MeleeAnim = true;
            this->MeleeAnimPercent = 0;
            this->MeleeAnimTexture = &game->Textures[CurrentWeapon->texture];
            this->MeleeAnimAngle = Angle - 90;
            if (Owner->Type == EnemyType) {
                float AngleToPlayer = atan2(game->MainPlayer->BoundingBox.y - Target.y, game->MainPlayer->BoundingBox.x - Target.x) * RAD2DEG;
                float Dist = Vector2Distance({game->MainPlayer->BoundingBox.x, game->MainPlayer->BoundingBox.y}, {Owner->BoundingBox.x, Owner->BoundingBox.y});
                if (AngleToPlayer - MeleeAnimRange/2 < Angle && AngleToPlayer + MeleeAnimRange/2 > Angle && Dist <= CurrentWeapon->Range) {
                    game->MainPlayer->Health -= CurrentWeapon->Damage;
                }
            } else {
                std::vector<shared_ptr<Entity>>* array = &game->Entities[EnemyType];
                for (int i = 0; i < array->size(); i++) {
                    if (shared_ptr<Entity> entity = array->at(i); entity != nullptr and !entity->ShouldDelete) {
                        float AngleToEntity = atan2(Owner->BoundingBox.y - entity->BoundingBox.y, Owner->BoundingBox.x - entity->BoundingBox.x) * RAD2DEG;
                        float Dist = Vector2Distance({entity->BoundingBox.x, entity->BoundingBox.y}, {Owner->BoundingBox.x, Owner->BoundingBox.y});
                        if (Dist <= CurrentWeapon->Range)
                        if (AngleToEntity - MeleeAnimRange/2 < Angle && AngleToEntity + MeleeAnimRange/2 > Angle && Dist <= CurrentWeapon->Range && Raycast(entity->BoundingBox)) {
                            entity->Health -= CurrentWeapon->Damage;
                            if (entity->Health <= 0) {
                                game->MainPlayer->Health += entity->MaxHealth / 2.0f;
                            }
                        }
                    }
                }
            }
        }

        AttackCooldown = 0;
    }
}

void WeaponsSystem::Equip(int Index) {
    if (Weapons->length() > Index && !Weapons[Index].empty()) {
        CurrentWeaponIndex = Index;
        CurrentWeapon = &game->Weapons[Weapons[CurrentWeaponIndex]];
    }
}

void WeaponsSystem::Unequip() {
    CurrentWeaponIndex = -1;
    CurrentWeapon = nullptr;
}
