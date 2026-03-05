//
// Created by lalit on 8/27/2025.
//

#include "Player.h"
#include <nlohmann/json.hpp>
#include "Enemy.h"
#include "../../game/Game.h"

#include "raylib.h"
#include <raymath.h>

using namespace std;

// the player's name is Jones
// little bouncing ballz boy!
Player::Player(float X, float Y, float Speed, Texture2D &PlayerTexture, Game &game) : Entity(PlayerTexture,
                                                                   Rectangle(X - 18, Y - 18, 36, 36), Speed, game) {
    this->Type = PlayerType;
    this->Kills = 0;
    this->isInvincible = false;
    this->LastKills = 0;
    this->OrigSpeed = Speed;
    this->ExtraSpeed = 0;
    this->IntervalHealth = Health;
    this->LastInterval = game.GetGameTime();
    this->LastWarningSign = game.GetGameTime();
    this->HealthConcern = false;
    this->WarningSign = false;
    this->SpeedBuff = 0;
    this->LastTanked = 0;
    this->damageNotifs = std::vector<Vector3>();
    this->LastPos = Vector2(0, 0);
    this->InvincibilityResetTimer = 0;
    this->PlayerDashLineThickness = 10;
    this->ShaderUniformLoc = GetShaderLocation(game.Shaders["dash_arrow"], "time");
}

Player::Player() {
}

Player::~Player() {

}

void Player::ToggleInvincibility() {
    this->isInvincible = !this->isInvincible;
}

void Player::DamageNotification(Vector2 from)
{
    this->damageNotifs.push_back({from.x, from.y, (float) game->GetGameTime()});
}

void Player::PhysicsUpdate(float dt, double time) {
    float MovementX = 0;
    float MovementY = 0;
    if (PlayerFrozenTimer <= 0) {
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
    }
    if (Health >= 200 && time - LastTanked >= 2)
    {
        Health -= 2;
        LastTanked = time;
    }
    if (VelocityPower < 50)
    {
        VelocityPower = 0;
    }
    Movement = Vector2(MovementX, MovementY);
    if (Vector2Distance(LastPos, {BoundingBox.x,BoundingBox.y}) > 0 && Vector2Distance({0,0}, Movement) > 0) {
        LastMovedTime = time;
        ExtraSpeed += 10 * dt;
    }
    LastPos = {BoundingBox.x,BoundingBox.y};
    if (time - LastMovedTime > 1)
        ExtraSpeed = 0;
    if (ReduceSpeedBuff)
    {
        SpeedBuff -= 100 * dt;
        if (SpeedBuff <= 0)
        {
            SpeedBuff = 0;
            ReduceSpeedBuff = false;
        }
    }
    ExtraSpeed = min(ExtraSpeed, 400.0f);
    Speed = (OrigSpeed + ExtraSpeed + SpeedBuff);
    if ((Health/MaxHealth) > 2.0f && !isInvincible)
        Speed *= (1.0f - min(((Health/MaxHealth)-3.0f) / 2.0f, 0.5f));
    if (VelocityPower > 0 && !Dodging) {
        // get enemies list
        std::vector<shared_ptr<Entity>>* array = &game->MainEntityManager.Entities[EnemyType];
        for (int i = 0; i < array->size(); i++) {
            if (shared_ptr<Enemy> entity = dynamic_pointer_cast<Enemy>(array->at(i)); entity != nullptr and !entity->ShouldDelete) {
                // have we already attacked them? if so, ignore this!!!
                bool already_attacked = false;
                for (auto e : DashedEnemies) {
                    if (!e.owner_before(entity) && !entity.owner_before(e)) {
                        already_attacked = true;
                        break;
                    }
                }

                // attack them
                AttackDashedEnemy(entity, already_attacked);
            }
        }
    } else {
        DashedEnemies.clear();
    }
    Entity::PhysicsUpdate(dt, time);
}

void Player::AttackDashedEnemy(std::shared_ptr<Enemy> entity, bool already_attacked) {
    // check if we're colliding with them. if so, attack!
    if (CheckCollisionRecs(BoundingBox, entity->BoundingBox) && !already_attacked) {
        // calculate damage & attack
        float Damage = VelocityPower / 22.5f;
        Damage *= min(max((Health / MaxHealth)-2.0f, 1.0f), 5.5f);
        if (entity->Armor <= 0)
            entity->Health -= Damage;
        else
        {
            entity->Armor -= Damage;
        }
        Health += Damage / 5.5f;

        float amount = 1500.0f;

        // did we kill them? if so, give health & kills
        if (entity->Health <= 0) {
            Health += Damage * 0.55f;
            //game->Slowdown(0.35f, VelocityPower / 1450.0f);
            amount= 950;
            Kills+=1;
            game->GameScore += 50;
        }

        game->GameScore += 10;
        game->MainCameraManager.CameraPosition += Vector2Normalize({(float)GetRandomValue(-25, 25), (float)GetRandomValue(-25, 25)}) * (VelocityPower / 150);
        game->MainCameraManager.ShakeCamera(VelocityPower / (amount - 50) / 1.5f);
        game->MainCameraManager.QuickZoom(0.95f, 0.05f, false);
        //game->Slowdown(0.5f);
        game->MainSoundManager.PlaySoundM("dash_hit",min(max(VelocityPower/amount, 0.0f), 1.0f));

        // give them pushback force
        entity->VelocityMovement = VelocityMovement;
        entity->VelocityPower = -VelocityPower/2;

        // increase velocity and mark enemy as attacked
        VelocityPower += VelocityPower / (amount/200);
        DashedEnemies.push_back(std::weak_ptr(entity));
    }
}

void Player::OnWallVelocityBump(float Power)
{
    if (Power > 400)
    {
        float PreviousH = Health;
        Entity::OnWallVelocityBump(Power);
        game->MainSoundManager.PlaySoundM("dash_wall_hit");
        float Damage = Power / 130.0f;
        if (Health - Damage >= 20)
        {
            Health -= Damage;
        } else if (PreviousH >= 20)
        {
            Health = 20;
        }
    }
}

void Player::DashLogic() {
    // update dash cooldownm
    if (DashCooldown > 0)
        DashCooldown -= game->GetGameDeltaTime();

    Vector2 WorldMousePos = GetScreenToWorld2D(GetMousePosition(), game->MainCameraManager.RaylibCamera);
    if (DashCooldown <= 0 && IsKeyDown(KEY_LEFT_SHIFT)) {
        if (!IsPreparingForDash) {
            DashTimeStart = game->GetGameTime();
            PlayerDashLineThickness = 10;
        }
        IsPreparingForDash = true;
    }
    if (IsPreparingForDash && !IsKeyDown(KEY_LEFT_SHIFT)) {
        IsPreparingForDash = false;
    }
    if (game->MainPlayer->IsPreparingForDash || game->MainPlayer->weaponsSystem.TimeStartedReloading != -1) {
        game->MainCameraManager.QuickZoom(1.25f, 0.1f);
        if (game->MainPlayer->IsPreparingForDash)
        {
            if (static_cast<float>(game->GetGameTime() - DashTimeStart) / 1.1f > 0.8f)
                PlayerDashLineThickness = Lerp(PlayerDashLineThickness, 20, 2 * game->GetGameDeltaTime());
            float alpha = static_cast<float>(game->GetGameTime() - DashTimeStart) / 1.1f;
            Vector2 Target = GetScreenToWorld2D(GetMousePosition(), game->MainCameraManager.RaylibCamera);
            float cx = BoundingBox.x + BoundingBox.width / 2;
            float cy = BoundingBox.y + BoundingBox.height / 2;
            float FinalAngle = (atan2(cy - Target.y, cx - Target.x) * RAD2DEG);
            Texture2D& MeleeAnimTexture = game->Textures["arrow"];
            float width = MeleeAnimTexture.width;
            float height = MeleeAnimTexture.height;
            BeginShaderMode(game->Shaders["dash_arrow"]);
            float t = static_cast<float>(game->GetGameTime() - DashTimeStart);
            SetShaderValue(game->Shaders["dash_arrow"],
                ShaderUniformLoc,
                &t,
                SHADER_ATTRIB_FLOAT);
            float size = 0.375f;
            DrawTexturePro(MeleeAnimTexture, Rectangle(0, 0, width, height *6),
                           Rectangle(BoundingBox.x + BoundingBox.width/2 - cosf((FinalAngle) * DEG2RAD)*10,
                               BoundingBox.y +BoundingBox.height/2 - sinf((FinalAngle) * DEG2RAD)*10, width*size,
                                     height * 6 * size), Vector2(width * size * 0.5, height*6*size), FinalAngle-90, ColorAlpha(WHITE, alpha));
            EndShaderMode();
        }
    }
    if ((IsMouseButtonDown(1) || IsMouseButtonDown(0)) && IsPreparingForDash && Health > 0 && game->GetGameTime() - DashTimeStart >= 0.35f) {
        DashCooldown = 1.1f + (2.2f - min(static_cast<float>(game->GetGameTime() - DashTimeStart), 1.1f) * 2);
        DashedEnemies.clear();
        VelocityMovement = Vector2Subtract(WorldMousePos, {BoundingBox.x, BoundingBox.y});
        VelocityPower = 2500.0f * max(min(static_cast<float>(game->GetGameTime() - DashTimeStart), 1.1f), 0.45f);
        VelocityPower /= min(max((Health / MaxHealth)-2.0f, 1.0f), 1.5f);
        game->MainSoundManager.PlaySoundM("dash");
        PlayerFrozenTimer = .5f * min(max((VelocityPower / 2500.0f), 0.35f), 1.1f);
        if (!isInvincible)
        {
            ToggleInvincibility();
            InvincibilityResetTimer= 0.1f;
        }
        if (IsMouseButtonDown(0)) {

            Dodging = true;
            InvincibilityResetTimer = 1.0f;
            DashCooldown = 3.0;
        }
        IsPreparingForDash = false;
    }

    // display dashing bar
    int w = 56;
    int h = 15;
    float a = 0;
    if (IsPreparingForDash)
        a = Lerp(0, 0.6f, min( (float)(game->GetGameTime() - DashTimeStart) / 0.2f, 1.0f ));

    DrawRectangle((int)(BoundingBox.x + (BoundingBox.width / 2) - (w/2)),
        (int)(BoundingBox.y + BoundingBox.width + 10),
        w, h, ColorAlpha(BLACK, a));

    DrawRectangle(
        (int)(BoundingBox.x + (BoundingBox.width / 2) - (w/2))+5,
        (int)(BoundingBox.y + BoundingBox.width + 10)+5,
        (!IsPreparingForDash ? 0 : min(static_cast<float>(game->GetGameTime() - DashTimeStart) / 1.1f, 1.0f))*(w-10),
        h-10,
        ColorAlpha(WHITE, a*1.25f));

    // Health bar
    DrawRectangle((int)(BoundingBox.x-20-h),
       (int)(BoundingBox.y + (BoundingBox.height / 2) - (w/2)),
       h, w,
       ColorAlpha(BLACK, 0.4f));

    DrawRectangle((int)(BoundingBox.x-20-h)+5,
       (int)(BoundingBox.y + (BoundingBox.height / 2) - (w/2))+5,
       h - 10, min(Health / 100, 1.0f) * (w - 10.0f),
       ColorAlpha(GREEN, (HealthConcern ? (WarningSign ? 0 : 0.8f) : 0.8f) ));

    // Ammo bar
    if (weaponsSystem.CurrentWeapon != nullptr && weaponsSystem.CurrentWeapon->Ammo > 0)
    {
        DrawRectangle((int)(BoundingBox.x+BoundingBox.height+20),
       (int)(BoundingBox.y + (BoundingBox.height / 2) - (w/2)),
       h, w,
       ColorAlpha(BLACK, 0.4f));

        DrawRectangle((int)(BoundingBox.x+BoundingBox.height+20)+5,
           (int)(BoundingBox.y + (BoundingBox.height / 2) - (w/2))+5,
           h - 10, ((float)weaponsSystem.WeaponAmmo[weaponsSystem.CurrentWeaponIndex] / weaponsSystem.CurrentWeapon->Ammo) * (w - 10.0f),
           ColorAlpha(YELLOW, 0.8f ));
    }
}

void Player::DisplayDamageNotifs()
{
    std::erase_if(damageNotifs, [this](Vector3 e)
    {
        return (float)game->GetGameTime() - e.z >= 1.5f;
    });
    for (Vector3 notif : damageNotifs)
    {
        float time = (float)game->GetGameTime() - notif.z;
        Vector2 m = Vector2Multiply(Vector2Normalize(Vector2Subtract({BoundingBox.x + BoundingBox.width/2, BoundingBox.y + BoundingBox.height/2}, {notif.x, notif.y})),
            {200, 200});
        float angle = 180 - Vector2LineAngle({0,0},m) * RAD2DEG;
        float trans = 1.0f;
        if (time <= 0.5f)
            trans = time / 0.5f;
        if (time >= 1.0f)
            trans = (time - 1.0f) / 0.5f;
        DrawCircleSector(Vector2Subtract({BoundingBox.x + BoundingBox.width/2, BoundingBox.y + BoundingBox.height/2}, m), 45, (180+angle)-50, (180+angle)+50, 10, ColorAlpha(WHITE, trans/2));
    }
}

void Player::Update() {

    // is the weapon system not initialized?? init it now!!!
    if (!this->weaponsSystemInit) {
        this->weaponsSystem = WeaponsSystem(shared_from_this(), *game);
        this->powerupSystem = PowerupSystem(dynamic_pointer_cast<Player>(shared_from_this()), *game);
        auto f = game->LevelData[game->CurrentLevelName]["inventory"];
        for (int i = 0; i < (int)min((float)f.size(),3.0f); i++) {
            this->weaponsSystem.Weapons[i] = f[i];
            this->weaponsSystem.WeaponAmmo[i] = game->Weapons[f[i]].Ammo;
        }
        if (game->Powerups.contains(game->LevelData[game->CurrentLevelName]["powerup"]))
        {
            powerupSystem.SetPowerup(game->Powerups[game->LevelData[game->CurrentLevelName]["powerup"]]);
        }
        this->weaponsSystem.Equip(0);
        this->weaponsSystemInit = true;
    }

    // warning sign interval
    if (game->GetGameTime() - LastInterval >= 1.5f)
    {
        HealthConcern = (IntervalHealth - Health) >= 75;
        LastInterval = game->GetGameTime();
    }
    if (Health < 50)
        HealthConcern = true;

    if (game->GetGameTime() - LastWarningSign >= 0.1f)
    {
        WarningSign = !WarningSign;
        LastWarningSign = game->GetGameTime();
    }

    if (Health > 0 && HealthConcern && WarningSign)
    {
        DrawTexturePro(game->Textures["warning"], {0,0,33,34},{BoundingBox.x + BoundingBox.width/2 + 12,BoundingBox.y - 24 - 10,24,24},{0,0},0,WHITE);
    }


    // player transparency processing
    EntityColor = ColorAlpha(WHITE, Alpha);
    Alpha = Lerp(Alpha, (InvincibilityResetTimer > 0 ? 0.5f : 1.0f), 5.5f*game->GetGameDeltaTime());

    // dashing logic
    DashLogic();

    if (InvincibilityResetTimer > 0)
        InvincibilityResetTimer -= game->GetGameDeltaTime();
    if (InvincibilityResetTimer <= 0)
    {
        isInvincible = false;
        Dodging=false;
    }

    if (PlayerFrozenTimer <= 0) {
        // powerup logic
        if (IsKeyDown(KEY_F))
        {
            powerupSystem.Activate();
            game->GameScore += 5;
        }

        // firing logic
        if (IsMouseButtonDown(0) && weaponsSystem.CurrentWeapon != nullptr &&
            weaponsSystem.CurrentWeapon->Ammo > 0 && weaponsSystem.WeaponAmmo[weaponsSystem.CurrentWeaponIndex] <=0 &&
            weaponsSystem.AttackCooldowns[weaponsSystem.CurrentWeaponIndex] >= weaponsSystem.CurrentWeapon->Cooldown)
        {
            weaponsSystem.Reload();
        }

        auto WorldMousePos = Vector2Add(GetMousePosition(), game->MainCameraManager.CameraPositionUnaffected);
        if (IsMouseButtonDown(0) && !IsPreparingForDash)
            weaponsSystem.Attack(WorldMousePos);

        // reload logic
        if (IsKeyPressed(KEY_R) && weaponsSystem.TimeStartedReloading == -1)
            weaponsSystem.Reload();

        //dropping stuff
        if (IsKeyPressed(KEY_Q) && weaponsSystem.CurrentWeaponIndex != -1)
            weaponsSystem.DropWeapon(weaponsSystem.Weapons[weaponsSystem.CurrentWeaponIndex]);

        // inventory input logic
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
    } else {
        PlayerFrozenTimer -= game->GetGameDeltaTime();
    }

    game->GameScore += Health * 0.005f * game->GetGameDeltaTime();

    // update entity
    Entity::Update();
    weaponsSystem.Update();
    powerupSystem.Update();
    DisplayDamageNotifs();

    // did we get a kill? play kill sound game!
    if (Kills != LastKills) {
        game->MainSoundManager.PlaySoundM("death");
        game->GameScore += 100;
    }
    LastKills = Kills;
}
