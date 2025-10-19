//
// Created by lalit on 9/14/2025.
//

#include "raylib.h"
#include "raymath.h"
#include "Entity.h"
#include "Bullet.h"
#include "Game.h"
#include "Weapons.h"

#include "Enemy.h"
#include "iostream"

using namespace std;

WeaponsSystem::WeaponsSystem(shared_ptr<Entity> Owner, Game& game) {

    // Set everything to basic, default values.

    CurrentWeapon = nullptr;
    this->game = &game;
    this->OwnerPtr = Owner;
    CurrentWeaponIndex = 0;
    MeleeAnim = false;
    MeleeAnimTexture = nullptr;
    MeleeAnimAngle = 0;
    MeleeAnimRange = 45;
    MeleeAnimPercent = 0;
    PointRemovalTimer = 0;
    MeleeAnimAlpha = 1;
    for (int i = 0; i < 3; i++) {
        AttackCooldowns[i] = 0;
        Weapons[i] = "";
    }
}

WeaponsSystem::WeaponsSystem() {
}

WeaponsSystem::~WeaponsSystem() {
}

void WeaponsSystem::DisplayGunTexture() {
    auto Owner = OwnerPtr.lock();
    float cx = Owner->BoundingBox.x + Owner->BoundingBox.width / 2;
    float cy = Owner->BoundingBox.y + Owner->BoundingBox.height / 2;
    float FinalAngle = (atan2(cy - (GetMouseY() + game->CameraPosition.y), cx - (GetMouseX() + game->CameraPosition.x)) * RAD2DEG);
    MeleeAnimTexture = &game->Textures[CurrentWeapon->texture];
    float width = MeleeAnimTexture->width*CurrentWeapon->Size;
    float height = MeleeAnimTexture->height*CurrentWeapon->Size;
    DrawTexturePro(*MeleeAnimTexture, Rectangle(0, 0, static_cast<float> (MeleeAnimTexture->width), static_cast<float> (MeleeAnimTexture->height)),
                   Rectangle(Owner->BoundingBox.x + Owner->BoundingBox.width/2 - game->CameraPosition.x - cosf((FinalAngle) * DEG2RAD)*CurrentWeapon->Range, Owner->BoundingBox.y + Owner->BoundingBox.height/2 - game->CameraPosition.y - sinf((FinalAngle) * DEG2RAD)*CurrentWeapon->Range, width,
                             height), Vector2(0, height / 2), FinalAngle, ColorAlpha(WHITE, MeleeAnimAlpha));
}

void WeaponsSystem::Update() {

    // Getting owner variable & updating cooldown info

    auto Owner = OwnerPtr.lock();
    for (int i = 0; i < 3; i++) {
        if (!Weapons[i].empty()) {
            AttackCooldowns[i] += GetFrameTime();
        } else {
            AttackCooldowns[i] = 0;
        }
    }

    // Display weapon cone if using melee weapon

    if (CurrentWeapon != nullptr && CurrentWeapon->isMelee && AttackCooldowns[CurrentWeaponIndex] >= CurrentWeapon->Cooldown && Owner->Type == PlayerType) {
        MeleeAnimRange = CurrentWeapon->AngleRange;
        // get angle
        float cx = Owner->BoundingBox.x + Owner->BoundingBox.width / 2;
        float cy = Owner->BoundingBox.y + Owner->BoundingBox.height / 2;
        float Angle = (atan2(cy - (GetMouseY() + game->CameraPosition.y), cx - (GetMouseX() + game->CameraPosition.x)) * RAD2DEG)+180;

        // get positions
        float z = (Angle - MeleeAnimRange/2) * DEG2RAD;
        float dist = CurrentWeapon->Range;
        Vector2 AnglePos1 = {cosf(z)*dist, sinf(z)*dist};
        z = (Angle + MeleeAnimRange/2) * DEG2RAD;
        Vector2 AnglePos2 = {cosf(z)*dist, sinf(z)*dist};

        // modify positions a little
        Vector2 OwnerPos = {cx, cy};
        AnglePos1.x += OwnerPos.x;
        AnglePos1.y += OwnerPos.y;
        AnglePos2.x += OwnerPos.x;
        AnglePos2.y += OwnerPos.y;

        // render cone
        DrawTriangle(AnglePos1 - game->CameraPosition, OwnerPos - game->CameraPosition, AnglePos2 - game->CameraPosition, ColorAlpha(WHITE, 0.5f));
    }

    // Display melee animation points

    if (points.size() != 0) {
        for (int i = 0; i < points.size(); i++) {
            if (i != 0) {
                Vector2 f1 = {points.at(i-1).x + Owner->BoundingBox.x + Owner->BoundingBox.width/2 - game->CameraPosition.x, points.at(i-1).y+Owner->BoundingBox.y + Owner->BoundingBox.height/2  - game->CameraPosition.y};
                Vector2 f = {points.at(i).x + Owner->BoundingBox.x + Owner->BoundingBox.width/2 - game->CameraPosition.x, points.at(i).y+ Owner->BoundingBox.y + Owner->BoundingBox.height/2 - game->CameraPosition.y};
                DrawLineBezier(f, f1, 10, WHITE);
            }
        }
    }

    // display gun tex
    if (CurrentWeapon != nullptr && (!CurrentWeapon->texture.empty()) && (!CurrentWeapon->isMelee))
        DisplayGunTexture();

    // Clear out melee animation points

    if (GetTime() - PointRemovalTimer >= 0.001 && points.size() > 0) {
        points.erase(points.begin()+points.size()-1);
        PointRemovalTimer = GetTime();
    }

    // Melee animation

    if (!MeleeAnim) {
        // set some basic values ready
        MeleeAnimTexture = nullptr;
        MeleeAnimAngle = 0;
        MeleeAnimRange = 90;
        MeleeAnimPercent = 0;
        MeleeAnimAlpha = 1;
    } else {
        // if we still have a weapon equipped, set the animation angle range
        if (CurrentWeapon != nullptr) {
            MeleeAnimRange = CurrentWeapon->AngleRange;
        }
        // increase the animation's progress
        if (MeleeAnimPercent <= 1.0)
            MeleeAnimPercent += 3 * GetFrameTime();

        // set the animation's state to stop/start with the progress
        MeleeAnim = MeleeAnimPercent <= 1.0 || MeleeAnimAlpha > 0;

        // animation variables for image
        float width = (MeleeAnimTexture->width * 1.35f);
        float height = (MeleeAnimTexture->height * 1.35f);
        float FinalAngle = MeleeAnimAngle - (MeleeAnimRange/2) + (MeleeAnimPercent * MeleeAnimRange);

        // adding points
        if (MeleeAnimPercent <= 1.0) {
            points.clear();
            float length = 30;
            int times = 25;
            // getting the point postions and adding them
            for (int i = 0; i < times; i++) {
                for (int x = 5; x < 10; x++) {
                    float ThisAngle = max(MeleeAnimAngle - (MeleeAnimRange/2), FinalAngle - ((length/times) * i));;
                    points.push_back({-cosf((ThisAngle+90) * DEG2RAD)*(20.4f)*x,
                        -sinf((ThisAngle+90) * DEG2RAD)*(20.4f)*x});
                }
            }
            MeleeAnimAlpha = 1;
        } else {
            // if the animation is already complete, phase out the sword
            MeleeAnimAlpha -= 3 * GetFrameTime();
        }

        // render sword
        DrawTexturePro(*MeleeAnimTexture, Rectangle(0, 0, static_cast<float> (MeleeAnimTexture->width), static_cast<float> (MeleeAnimTexture->height)),
                   Rectangle(Owner->BoundingBox.x + Owner->BoundingBox.width/2 - game->CameraPosition.x - cosf((FinalAngle+90) * DEG2RAD)*150, Owner->BoundingBox.y + Owner->BoundingBox.height/2 - game->CameraPosition.y - sinf((FinalAngle+90) * DEG2RAD)*150, width,
                             height), Vector2(width / 2, height / 2), FinalAngle, ColorAlpha(WHITE, MeleeAnimAlpha));

    }
}

void WeaponsSystem::MeleeAttack(std::shared_ptr<Entity> entity, float Angle) {
    auto Owner = OwnerPtr.lock();
    // Get angle and distance from victim entity
    float AngleToEntity = atan2(Owner->BoundingBox.y - entity->BoundingBox.y, Owner->BoundingBox.x - entity->BoundingBox.x) * RAD2DEG;
    float Dist = Vector2Distance({entity->BoundingBox.x, entity->BoundingBox.y}, {Owner->BoundingBox.x, Owner->BoundingBox.y});

    // get center of bboxes
    float owner_cx = Owner->BoundingBox.x + Owner->BoundingBox.width / 2;
    float owner_cy = Owner->BoundingBox.y + Owner->BoundingBox.height / 2;

    float cx = entity->BoundingBox.x + entity->BoundingBox.width / 2;
    float cy = entity->BoundingBox.y + entity->BoundingBox.height / 2;

    // if enemy is in sight & within range, attack!
    if (AngleToEntity - MeleeAnimRange/2 < Angle && AngleToEntity + MeleeAnimRange/2 > Angle && Dist <= CurrentWeapon->Range && game->RayCast({owner_cx, owner_cy}, {cx, cy})) {

        if (entity->Type == EnemyType) { // if victim is enemy, check for armor damage
            shared_ptr<Enemy> enemy = dynamic_pointer_cast<Enemy>(entity);
            if (enemy->Armor <= 0)
                enemy->Health -= CurrentWeapon->Damage;
            else
                enemy->Armor -= CurrentWeapon->Damage;
        } else { // if they are normal, just damage them normally
            entity->Health -= CurrentWeapon->Damage;
        }

        // if entity dies, give owner health and increase kill count for player
        if (entity->Health <= 0) {
            if (Owner->Health > 0)
                Owner->Health += entity->MaxHealth / 2.0f;
            if (Owner->Type == PlayerType)
                game->MainPlayer->Kills += 1;
        }
    }
}


void WeaponsSystem::Attack(Vector2 Target) {
    auto Owner = OwnerPtr.lock();
    if (CurrentWeapon != nullptr && AttackCooldowns[CurrentWeaponIndex] >= CurrentWeapon->Cooldown) {

        // Get angle + fire point
        float TargetAngle = atan2(Owner->BoundingBox.y - Target.y, Owner->BoundingBox.x - Target.x) * RAD2DEG;
        float cX = -cos(TargetAngle * (2 * PI / 360))*CurrentWeapon->Range;
        float cY = -sin(TargetAngle * (2 * PI / 360))*CurrentWeapon->Range;
        cX += Owner->BoundingBox.x + (Owner->BoundingBox.width / 2);
        cY += Owner->BoundingBox.y + (Owner->BoundingBox.height / 2);

        float Owner_cX = Owner->BoundingBox.x + (Owner->BoundingBox.width / 2);
        float Owner_cY = Owner->BoundingBox.y + (Owner->BoundingBox.height / 2);

        bool Valid = game->RayCast({Owner_cX, Owner_cY}, {cX, cY});

        // Play weapon sound
        if (game->Sounds.contains(CurrentWeapon->sound) && Owner->Type == PlayerType && (CurrentWeapon->isMelee || Valid)) {
            Sound* s = &game->Sounds[CurrentWeapon->sound];
            SetSoundVolume(*s, CurrentWeapon->Intensity);
            PlaySound(*s);
        }

        // Shake camera
        if (CurrentWeapon->ShakeScreen && Owner->Type == PlayerType && (CurrentWeapon->isMelee || Valid))
            game->ShakeCamera(CurrentWeapon->Intensity);

        // Gun attack
        if (Valid && !CurrentWeapon->isMelee) {

            // loop thru requested shots
            for (int i = 1; i < CurrentWeapon->Bullets+1; i++) {

                float Angle = TargetAngle;
                if (CurrentWeapon->Bullets != 0)
                    Angle += ((CurrentWeapon->AngleRange / CurrentWeapon->Bullets)*i) - CurrentWeapon->AngleRange/2.0f; // get offset angle of shot

                // create bullet with weapon settings
                shared_ptr<Bullet> bullet = make_shared<Bullet>(cX, cY, Angle, CurrentWeapon->Size, CurrentWeapon->Speed, CurrentWeapon->Damage,
                                                                game->Textures["bullet"], Owner, *game);
                bullet->SlowdownOverTime = CurrentWeapon->SlowdownOverTime;
                game->Entities[BulletType].push_back(bullet);
            }

            // if the weapon is powerful enough to shake the screen, its prob powerful enough to move the player a little bit
            if (CurrentWeapon->ShakeScreen) {
                Owner->VelocityMovement = {cos(TargetAngle * (2 * PI / 360))*100,sin(TargetAngle * (2 * PI / 360))*100};
                Owner->VelocityPower = 450;
            }
        } else if (CurrentWeapon->isMelee) { // Melee attack
            // Get angle, set basic starting variables
            this->MeleeAnim = true;
            this->MeleeAnimPercent = 0;
            this->MeleeAnimTexture = &game->Textures[CurrentWeapon->texture];
            this->MeleeAnimAngle = TargetAngle - 90;

            // player attack check
            if (Owner->Type != PlayerType)
                MeleeAttack(game->MainPlayer, TargetAngle);

            // Loop through all enemies in game
            std::vector<shared_ptr<Entity>>* array = &game->Entities[EnemyType];
            for (int i = 0; i < array->size(); i++) {
                if (shared_ptr<Enemy> entity = dynamic_pointer_cast<Enemy>(array->at(i)); entity != nullptr and !entity->ShouldDelete) {
                    MeleeAttack(entity, TargetAngle);
                }
            }
        }

        // Reset cooldown
        AttackCooldowns[CurrentWeaponIndex] = 0;
    }
}

void WeaponsSystem::Equip(int Index) {
    // if weapon exists and we have space, equip it
    if (Weapons->length() > Index && !Weapons[Index].empty()) {
        CurrentWeaponIndex = Index;
        CurrentWeapon = &game->Weapons[Weapons[CurrentWeaponIndex]];
    }
}

void WeaponsSystem::Unequip() {
    // simply set the current weapon to nothing
    CurrentWeaponIndex = -1;
    CurrentWeapon = nullptr;
}
