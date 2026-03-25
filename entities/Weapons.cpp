//
// Created by lalit on 9/14/2025.
//

#include "raylib.h"
#include "raymath.h"
#include "Entity.h"
#include "subentities/Bullet.h"
#include "../game/Game.h"
#include "Weapons.h"
#include "subentities/Player.h"
#include "subentities/Enemy.h"
#include "iostream"
#include "subentities/Turret.h"

using namespace std;

// bro im finna have to rewrite like 99% of melee wep code in a bit :sob:
WeaponsSystem::WeaponsSystem(shared_ptr<Entity> Owner, Game& game) {

    // Set everything to basic, default values.

    CurrentWeapon = nullptr;
    this->game = &game;
    this->OwnerPtr = Owner;
    CurrentWeaponIndex = -1;
    MeleeAnim = false;
    MeleeAnimTexture = nullptr;
    MeleeAnimAngle = 0;
    MeleeAnimRange = 45;
    MeleeAnimPercent = 0;
    TimeStartedReloading = -1;
    MeleeAnimAlpha = 1;
    for (int i = 0; i < 3; i++) {
        AttackCooldowns[i] = 0;
        Weapons[i] = "";
        WeaponAmmo[i] = 0;
    }
}

WeaponsSystem::WeaponsSystem() {
}

WeaponsSystem::~WeaponsSystem() {
}

void WeaponsSystem::DisplayGunTexture() { // HATSUNE MIKU!!!!
    auto Owner = OwnerPtr.lock();
    Vector2 Target = GetScreenToWorld2D(GetMousePosition(), game->GameCamera.RaylibCamera);
    float Range = CurrentWeapon->Range;
    MeleeAnimTexture = &game->GameResources.Textures[CurrentWeapon->texture];
    float width = MeleeAnimTexture->width*CurrentWeapon->WeaponSize;
    float height = MeleeAnimTexture->height*CurrentWeapon->WeaponSize;
    if (Owner->Type == EnemyType)
        Target = {game->MainPlayer->BoundingBox.x + game->MainPlayer->BoundingBox.width/2,
        game->MainPlayer->BoundingBox.y + game->MainPlayer->BoundingBox.height/2
        };
    else if (Owner->Type == TurretType)
    {
        std::shared_ptr<Turret> ptr = dynamic_pointer_cast<Turret>(Owner);
        Target =ptr->Target;
        Range = width/2;
    }
    float cx = Owner->BoundingBox.x + Owner->BoundingBox.width / 2;
    float cy = Owner->BoundingBox.y + Owner->BoundingBox.height / 2;
    float FinalAngle = (atan2(cy - Target.y, cx - Target.x) * RAD2DEG);
    DrawTexturePro(*MeleeAnimTexture, Rectangle(0, 0, static_cast<float> (MeleeAnimTexture->width), static_cast<float> (MeleeAnimTexture->height)),
                   Rectangle(Owner->BoundingBox.x + Owner->BoundingBox.width/2 - (cosf((FinalAngle) * DEG2RAD)*Range), Owner->BoundingBox.y + Owner->BoundingBox.height/2 - (sinf((FinalAngle) * DEG2RAD)*Range), width,
                             height), Vector2(0, height / 2), FinalAngle, WHITE);
} // LOVELY CAVITY!!!

bool WeaponsSystem::GiveWeapon(std::string WeaponName, int Ammo)
{
    for (int i = 0; i < 3; i++)
    {
        if (Weapons[i].empty())
        {
            Weapons[i] = WeaponName;
            WeaponAmmo[i] = (Ammo == -1) ? game->GameResources.Weapons[WeaponName].Ammo : Ammo;
            return true;
        }
    }
    return false;
}

bool WeaponsSystem::DropWeapon(std::string WeaponName)
{
    if (!WeaponName.empty())
    {
        Vector2 DropLoc = {0, 0};
        auto Owner = OwnerPtr.lock();

        if (Owner != nullptr) {
            float dirX = (GetRandomValue(1, 2) == 1) ? 1 : -1;
            float dirY = (GetRandomValue(1, 2) == 1) ? 1 : -1;

            DropLoc = {Owner->BoundingBox.x - Owner->BoundingBox.width/2, Owner->BoundingBox.y - Owner->BoundingBox.height/2};
            DropLoc = Vector2Add(DropLoc, {
                (float)GetRandomValue(0, Owner->BoundingBox.width) * dirX * 1.5f,
                    (float)GetRandomValue(0, Owner->BoundingBox.height) * dirY * 1.5f
            });
        }

        for (int i = 0; i < 3; i++)
        {
            if (Weapons[i] == WeaponName)
            {
                if (CurrentWeaponIndex == i)
                    Unequip();
                game->PlaceWeaponPickup({
                    DropLoc,
                    GREEN,
                    50,
                    WeaponAmmo[i],
                    WeaponName,
                    2,
                    25
                });
                Weapons[i].clear();
                return true;
            }
        }
    }
    return false;
}

void WeaponsSystem::ResetMeleeAnim()
{
    MeleeAnimTexture = nullptr;
    MeleeAnimAngle = 0;
    MeleeAnimRange = 90;
    MeleeAnimPercent = 0;
    MeleeAnimAlpha = 1;
}

void WeaponsSystem::DisplayWeaponCone()
{
    auto Owner = OwnerPtr.lock();
    // Display weapon cone if using melee weapon
    if (CurrentWeapon != nullptr && CurrentWeapon->isMelee && (Owner->Type == PlayerType || Owner->Type == EnemyType) && Vector2Distance(Owner->GetCenter(), game->MainPlayer->GetCenter()) < CurrentWeapon->Range + GetScreenWidth()/2)
    {
        float cx = Owner->BoundingBox.x + Owner->BoundingBox.width / 2;
        float cy = Owner->BoundingBox.y + Owner->BoundingBox.height / 2;
        if (AttackCooldowns[CurrentWeaponIndex] >= CurrentWeapon->Cooldown && Owner->Type == PlayerType)
            MeleeDisplayRenderTarget = Vector2Subtract(GetScreenToWorld2D(GetMousePosition(), game->GameCamera.RaylibCamera), Vector2{cx,cy});
        else if (AttackCooldowns[CurrentWeaponIndex] >= CurrentWeapon->Cooldown)
            MeleeDisplayRenderTarget = game->MainPlayer->GetCenter();

        Vector2 MP = Vector2Add(MeleeDisplayRenderTarget, Vector2{cx,cy});

        MeleeAnimRange = CurrentWeapon->AngleRange;
        // get angle
        float Angle = (atan2(cy - MP.y, cx - MP.x) * RAD2DEG)+180;

        float LeftAngle = (Angle - MeleeAnimRange/2);
        float Dist = CurrentWeapon->Range;

        //DrawCircleSector({cx,cy}, Dist, LeftAngle, RightAngle, 40, ColorAlpha(WHITE, MeleeAnimAlpha/2.0f));
        float opti = CurrentWeapon->AngleRange / 18.0f;
        if (Owner->Type == PlayerType)
            opti = 1.5f;
        for (int i = 0; i < MeleeAnimRange / opti; i++)
        {
            float Angle = LeftAngle + i * opti;
            float X = cos(Angle * (2 * PI / 360))*Dist;
            float Y = sin(Angle * (2 * PI / 360))*Dist;
            std::pair<bool,Vector2> p = game->RayCastPoint({cx,cy},{cx+X,cy+Y});
            DrawCircleSector({cx,cy}, Vector2Distance({cx,cy},p.second), Angle - opti/2, Angle + opti/2, 1, ColorAlpha(WHITE, MeleeAnimAlpha/2.0f));
        }
    }
}

void WeaponsSystem::Update() {

    // Getting owner variable & updating cooldown info & updating ammo info
    // aarush was here

    auto Owner = OwnerPtr.lock();

    if (Owner != nullptr && CurrentWeapon != nullptr)
        Owner->WeaponWeightSpeedMultiplier = CurrentWeapon->WeaponWeightSpeedMultiplier;

    for (int i = 0; i < 3; i++) {
        if (!Weapons[i].empty()) {
            AttackCooldowns[i] += game->GetGameDeltaTime();
        } else {
            AttackCooldowns[i] = 0;
            WeaponAmmo[i] = 0;
        }
    }

    // reload weps
    if (CurrentWeapon != nullptr && TimeStartedReloading != -1 && game->GetGameTime() - TimeStartedReloading >= CurrentWeapon->ReloadTime)
    {
        WeaponAmmo[CurrentWeaponIndex] = CurrentWeapon->Ammo;
        TimeStartedReloading = -1;
    }

    DisplayWeaponCone();

    // display gun tex
    if (CurrentWeapon != nullptr && (!CurrentWeapon->texture.empty()) && (!CurrentWeapon->isMelee) && Owner->IsVisible())
        DisplayGunTexture();

    DisplayMeleeAnim();
}

void WeaponsSystem::DisplayMeleeAnim()
{
    auto Owner = OwnerPtr.lock();
    // Melee animation (signma)

    if (!MeleeAnim) {
        // set some basic values ready
        ResetMeleeAnim();
    } else if ((CurrentWeapon == nullptr || CurrentWeapon->isMelee) && MeleeAnimTexture != nullptr) {
        // if we still have a weapon equipped, set the animation angle range
        if (CurrentWeapon != nullptr) {
            MeleeAnimRange = CurrentWeapon->AngleRange;
        }
        // increase the animation's progress
        if (MeleeAnimPercent <= 1.0)
            MeleeAnimPercent += 3 * game->GetGameDeltaTime();

        // set the animation's state to stop/start with the progress
        MeleeAnim = MeleeAnimPercent <= 1.0 || MeleeAnimAlpha > 0;

        // animation variables for image
        float width = (MeleeAnimTexture->width * 1.35f * CurrentWeapon->WeaponSize);
        float height = (MeleeAnimTexture->height * 1.35f * CurrentWeapon->WeaponSize);
        float FinalAngle = MeleeAnimAngle - (MeleeAnimRange/2) + (MeleeAnimPercent * MeleeAnimRange);

        // adding points
        if (MeleeAnimPercent <= 1.0) {
            MeleeAnimAlpha = 1;
        } else {
            // if the animation is already complete, phase out the sword
            MeleeAnimAlpha -= 3 * game->GetGameDeltaTime();
        }

        // render sword
        DrawTexturePro(*MeleeAnimTexture, Rectangle(0, 0, static_cast<float> (MeleeAnimTexture->width), static_cast<float> (MeleeAnimTexture->height)),
                   Rectangle(Owner->BoundingBox.x + Owner->BoundingBox.width/2 - cosf((FinalAngle+90) * DEG2RAD)*150, Owner->BoundingBox.y + Owner->BoundingBox.height/2 - sinf((FinalAngle+90) * DEG2RAD)*150, width,
                             height), Vector2(width / 2, height / 2), FinalAngle, ColorAlpha(WHITE, MeleeAnimAlpha));

    }
}

void WeaponsSystem::MeleeAttack(std::shared_ptr<Entity> entity, float Angle) {
    auto Owner = OwnerPtr.lock();
    // Get angle and distance from victim entity
    float AngleToEntity = atan2(Owner->GetCenter().y - entity->GetCenter().y, Owner->GetCenter().x - entity->GetCenter().x) * RAD2DEG;
    float Dist = Vector2Distance(entity->GetCenter(), Owner->GetCenter());

    float AngleDifference = Angle - AngleToEntity;
    if (AngleDifference < 180)
        AngleDifference += 360;
    if (AngleDifference > 180)
        AngleDifference -= 360;

    // if enemy is in sight & within range, attack!
    if (abs(AngleDifference) <= CurrentWeapon->AngleRange/2 && Dist <= CurrentWeapon->Range && game->RayCast(Owner->GetCenter(), entity->GetCenter()))
        Owner->DamageOther(entity, CurrentWeapon->Damage, nullptr, CurrentWeapon->HealthGain);
    else if (Owner->Type == PlayerType && game->RayCast(Owner->GetCenter(), entity->GetCenter()))
    {
        cout << "DEBUG" << endl;
        cout << (abs(AngleDifference) <= CurrentWeapon->AngleRange/2) << endl;
        cout << (Dist <= CurrentWeapon->Range) << endl;
        cout << AngleToEntity << " " << Angle << endl;
    }
}

void WeaponsSystem::GunAttack(float TargetAngle, float cX, float cY)
{
    auto Owner = OwnerPtr.lock();
    std::string BulletTexture = (!CurrentWeapon->BulletTexture.empty() && game->GameResources.Textures.contains(CurrentWeapon->BulletTexture))
            ? CurrentWeapon->BulletTexture : "bullet";

    float BulletLifetime = 8.5f;
    if (CurrentWeapon->BulletLifetime != -1)
        BulletLifetime = CurrentWeapon->BulletLifetime;

    // loop thru requested shots
    for (int i = 1; i < CurrentWeapon->Bullets+1; i++) {

        float Angle = TargetAngle + (float)GetRandomValue(CurrentWeapon->SpreadRange[0], CurrentWeapon->SpreadRange[1]);
        if (CurrentWeapon->Bullets > 1)
            Angle += ((CurrentWeapon->AngleRange / CurrentWeapon->Bullets)*i) - CurrentWeapon->AngleRange/2.0f; // get offset angle of shot

        // create bullet with weapon settings
        shared_ptr<Bullet> bullet = make_shared<Bullet>(cX, cY, Angle, CurrentWeapon->Size, CurrentWeapon->Speed, CurrentWeapon->Damage, BulletLifetime,
                                                        game->GameResources.Textures[BulletTexture], Owner, *game);
        bullet->SlowdownOverTime = CurrentWeapon->SlowdownOverTime;
        bullet->HealthGain = CurrentWeapon->HealthGain;
        if (Owner->Type == PlayerType)
            bullet->EntityColor = {255, 180, 255, 255};
        if (Owner->Type == EnemyType)
            bullet->EntityColor = {255, 182, 217, 255};
        game->GameEntities.AddEntity(BulletType, bullet);
    }

    // pushback character
    if (CurrentWeapon->PushbackForce != 0) {
        Owner->VelocityMovement = {cos(TargetAngle * (2 * PI / 360))*100,sin(TargetAngle * (2 * PI / 360))*100};
        Owner->VelocityPower = CurrentWeapon->PushbackForce;
    }
}

void WeaponsSystem::Attack(Vector2 Target) {
    auto Owner = OwnerPtr.lock();
    if (CurrentWeapon != nullptr && AttackCooldowns[CurrentWeaponIndex] >= CurrentWeapon->Cooldown &&
        ( (CurrentWeapon->Ammo > 0 && WeaponAmmo[CurrentWeaponIndex] > 0) || CurrentWeapon->Ammo <= 0 ) &&
        TimeStartedReloading == -1) {

        // Get angle + fire point
        float TargetAngle = atan2(Owner->BoundingBox.y - Target.y, Owner->BoundingBox.x - Target.x) * RAD2DEG;
        float cX = -cos(TargetAngle * (2 * PI / 360))*CurrentWeapon->Range;
        float cY = -sin(TargetAngle * (2 * PI / 360))*CurrentWeapon->Range;
        cX += Owner->BoundingBox.x + (Owner->BoundingBox.width / 2);
        cY += Owner->BoundingBox.y + (Owner->BoundingBox.height / 2);

        float Owner_cX = Owner->BoundingBox.x + (Owner->BoundingBox.width / 2);
        float Owner_cY = Owner->BoundingBox.y + (Owner->BoundingBox.height / 2);

        bool Valid = game->RayCast({Owner_cX, Owner_cY}, {cX, cY});

        std::string s = CurrentWeapon->sound[GetRandomValue(0, CurrentWeapon->sound.size()-1)];

        // Play weapon sound
        if (game->GameSounds.Sounds.contains(s) && (CurrentWeapon->isMelee || Valid)) {
            float Distance = Vector2Distance({Owner_cX, Owner_cY}, Vector2Add(game->GameCamera.CameraPosition, {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f}));

            float DistanceMultiplier = (1000.0f - Distance) / 1000.0f;
            DistanceMultiplier += GetRandomValue(-20, 20) / 100.0f;

            game->GameSounds.PlaySoundM(s, CurrentWeapon->Intensity * DistanceMultiplier,
                1.0f + GetRandomValue(-20, 20) / 100.0f);
        }

        // Shake camera
        if (CurrentWeapon->ShakeScreen && Owner->Type == PlayerType && (CurrentWeapon->isMelee || Valid))
            game->GameCamera.ShakeCamera(CurrentWeapon->Intensity);

        // Gun attack
        if (Valid && !CurrentWeapon->isMelee) {
            GunAttack(TargetAngle, cX, cY);
        } else if (CurrentWeapon->isMelee) { // Melee attack
            // Get angle, set basic starting variables
            this->MeleeAnim = true;
            this->MeleeAnimPercent = 0;
            this->MeleeAnimTexture = &game->GameResources.Textures[CurrentWeapon->texture];
            this->MeleeAnimAngle = TargetAngle - 90;

            // player attack check
            if (Owner->Type != PlayerType)
                MeleeAttack(game->MainPlayer, TargetAngle);

            // Loop through all enemies in game
            std::vector<shared_ptr<Entity>>* array = &game->GameEntities.Entities[EnemyType];
            for (int i = 0; i < array->size(); i++) {
                if (shared_ptr<Enemy> entity = dynamic_pointer_cast<Enemy>(array->at(i)); entity != Owner && entity != nullptr && !entity->ShouldDelete) {
                    MeleeAttack(entity, TargetAngle);
                }
            }
        }

        // Reset cooldown
        if ((Valid && !CurrentWeapon->isMelee) || CurrentWeapon->isMelee)
            AttackCooldowns[CurrentWeaponIndex] = 0;
        if (Valid && CurrentWeapon->Ammo > 0 && WeaponAmmo[CurrentWeaponIndex] > 0)
            WeaponAmmo[CurrentWeaponIndex] -= 1;
    }
}

void WeaponsSystem::Equip(int Index) {
    // if weapon exists and we have space, equip it
    if (CurrentWeaponIndex != Index && !Weapons[Index].empty()) {
        CurrentWeaponIndex = Index;
        CurrentWeapon = &game->GameResources.Weapons[Weapons[CurrentWeaponIndex]];
        TimeStartedReloading = -1;
    }
}

void WeaponsSystem::Unequip() {
    // simply set the current weapon to nothing
    CurrentWeaponIndex = -1;
    CurrentWeapon = nullptr;
    ResetMeleeAnim();
    TimeStartedReloading = -1;
    auto Owner = OwnerPtr.lock();
    if (Owner != nullptr)
        OwnerPtr.lock()->WeaponWeightSpeedMultiplier = 1;
}

void WeaponsSystem::Reload()
{
    if (CurrentWeapon != nullptr && CurrentWeapon->Ammo > 0 && TimeStartedReloading == -1)
        TimeStartedReloading = game->GetGameTime();
}
