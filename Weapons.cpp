//
// Created by lalit on 9/14/2025.
//

#include "raylib.h"
#include "Entity.h"
#include "Bullet.h"
#include "Game.h"
#include "Weapons.h"

using namespace std;

WeaponsSystem::WeaponsSystem(std::shared_ptr<Entity> Owner, Game& game) {
    AttackAmmo = -1;
    CurrentWeapon = nullptr;
    this->game = &game;
    this->Owner = Owner;
    AttackCooldown = 0;
    CurrentWeaponIndex = 0;
}

WeaponsSystem::WeaponsSystem() {
}

WeaponsSystem::~WeaponsSystem() {
}

void WeaponsSystem::Update() {
    if (AttackAmmo == -1 && CurrentWeapon != nullptr)
        AttackAmmo = CurrentWeapon->Ammo;
    AttackCooldown += GetFrameTime();
}

void WeaponsSystem::Attack(Vector2 Target) {
    if (CurrentWeapon != nullptr && AttackCooldown >= CurrentWeapon->Cooldown && (AttackAmmo > 0 || CurrentWeapon->Ammo == -1)) {
        if (!CurrentWeapon->isMelee) {
            auto OwnerPtr = Owner.lock();
            Target.x -= OwnerPtr->BoundingBox.x + (OwnerPtr->BoundingBox.width / 2);
            Target.y -= OwnerPtr->BoundingBox.y + (OwnerPtr->BoundingBox.height / 2);
            shared_ptr<Bullet> bullet = make_shared<Bullet>(OwnerPtr->BoundingBox.x + (OwnerPtr->BoundingBox.width / 2), OwnerPtr->BoundingBox.y + (OwnerPtr->BoundingBox.height / 2), Target, CurrentWeapon->Speed, CurrentWeapon->Damage,
                                                            game->Textures["bullet"], OwnerPtr, *game);
            game->Bullets.push_back(bullet);
            if (CurrentWeapon->Ammo != -1)
                AttackAmmo -= 1;
        }

        AttackCooldown = 0;
    }
}

void WeaponsSystem::Equip(int Index) {
    if (Weapons->length() > Index) {
        CurrentWeaponIndex = Index;
        CurrentWeapon = &game->Weapons[Weapons[CurrentWeaponIndex]];
    }
}

void WeaponsSystem::Unequip() {
    CurrentWeaponIndex = -1;
    CurrentWeapon = nullptr;
}
