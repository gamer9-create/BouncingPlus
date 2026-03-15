//
// Created by lalit on 3/14/2026.
//

#include "PlayerLogicProcessor.h"

#include <raymath.h>
#include <nlohmann/json.hpp>

#include "../../game/Game.h"
#include "Player.h"
#include "Bullet.h"

PlayerLogicProcessor::PlayerLogicProcessor(std::weak_ptr<Player> Owner)
{
    this->Owner = Owner;
    DashCooldown = 0;
    PlayerDashLineThickness = 10;
    DashTimeStart = 0;
    DamageNotifs = std::vector<Vector3>();
    DashedEnemies = std::vector<std::weak_ptr<Enemy>>();
}

PlayerLogicProcessor::PlayerLogicProcessor()
{
}

PlayerLogicProcessor::~PlayerLogicProcessor()
{
}

void PlayerLogicProcessor::Update()
{
    DashLogic();
    DisplayDamageNotifs();
}

void PlayerLogicProcessor::PhysicsUpdate()
{
    DashAttacking();
}

void PlayerLogicProcessor::Destroy()
{
    DashCooldown = 0;
    PlayerDashLineThickness = 10;
    DashTimeStart = 0;
    DamageNotifs.clear();
    DashedEnemies.clear();
}

void PlayerLogicProcessor::DamageNotification(Vector2 From)
{
    auto MyPlayer = Owner.lock();
    this->DamageNotifs.push_back({From.x, From.y, (float) MyPlayer->game->GetGameTime()});
}

void PlayerLogicProcessor::AttackDashedEnemy(std::shared_ptr<Enemy> entity, bool already_attacked)
{
    auto MyPlayer = Owner.lock();
    // check if we're colliding with them. if so, attack!
    float extra_size = 32;
    Rectangle myRect = Rectangle{MyPlayer->BoundingBox.x - extra_size / 2, MyPlayer->BoundingBox.y - extra_size / 2, MyPlayer->BoundingBox.width + extra_size, MyPlayer->BoundingBox.height + extra_size};
    if (MyPlayer->game->DebugDraw)
        DrawRectangleRec(myRect, ColorAlpha(RED, 0.5f));
    if (CheckCollisionRecs(myRect, entity->BoundingBox) && !already_attacked) {
        // calculate damage & attack
        float Damage = MyPlayer->VelocityPower / 22.5f;

        float EnemyConcentration = 1.0;

        std::vector<shared_ptr<Entity>> enemyArray = MyPlayer->game->MainEntityManager.Entities[EnemyType];
        for (int i = 0; i < enemyArray.size(); i++) {
            if (shared_ptr<Enemy> entity = dynamic_pointer_cast<Enemy>(enemyArray.at(i)); entity != nullptr and !entity->ShouldDelete) {
                if (Vector2Distance({entity->BoundingBox.x, entity->BoundingBox.y},{MyPlayer->game->MainPlayer->BoundingBox.x,MyPlayer->game->MainPlayer->BoundingBox.y}) < 550)
                {
                    EnemyConcentration += 0.06f;
                    if (EnemyConcentration >= 1.25f)
                        break;
                }
            }
        }

        std::vector<shared_ptr<Entity>> bulletArray = MyPlayer->game->MainEntityManager.Entities[BulletType];
        for (int i = 0; i < bulletArray.size(); i++) {
            if (shared_ptr<Bullet> entity = dynamic_pointer_cast<Bullet>(bulletArray.at(i)); entity != nullptr and !entity->ShouldDelete) {
                if (Vector2Distance({entity->BoundingBox.x, entity->BoundingBox.y},{MyPlayer->game->MainPlayer->BoundingBox.x,MyPlayer->game->MainPlayer->BoundingBox.y}) < 400)
                {
                    EnemyConcentration += 0.01f;
                    if (EnemyConcentration >= 1.25f)
                        break;
                }
            }
        }

        EnemyConcentration = min(EnemyConcentration, 1.25f);
        EnemyConcentration *= MyPlayer->game->LevelData[MyPlayer->game->CurrentLevelName]["player"]["dash_concentration_boost"].get<float>();

        Damage *= EnemyConcentration;
        Damage *= min(max((MyPlayer->Health / MyPlayer->MaxHealth)-2.0f, 1.0f), 5.5f);
        if (entity->Armor <= 0)
            entity->Health -= Damage;
        else
        {
            entity->Armor -= Damage;
        }

        float reward = Damage / 6.0f;
        MyPlayer->Health += reward;

        float amount = 1500.0f;

        // did we kill them? if so, give health & kills
        if (entity->Health <= 0) {
            MyPlayer->Health += Damage * 0.1f;
            //game->Slowdown(0.35f, VelocityPower / 1450.0f);
            amount = 950;
            MyPlayer->Kills+=1;
            MyPlayer->game->GameScore += 25;
        }

        MyPlayer->game->GameScore += 10;
        MyPlayer->game->MainCameraManager.CameraPosition += Vector2Normalize({(float)GetRandomValue(-25, 25), (float)GetRandomValue(-25, 25)}) * (MyPlayer->VelocityPower / 150);
        MyPlayer->game->MainCameraManager.ShakeCamera(MyPlayer->VelocityPower / (amount - 50) / 1.5f);
        MyPlayer->game->MainCameraManager.QuickZoom(0.95f, 0.05f, false);
        //game->Slowdown(0.5f);
        MyPlayer->game->MainSoundManager.PlaySoundM("dash_hit",min(max(MyPlayer->VelocityPower/amount, 0.0f), 0.8f));

        // give them pushback force
        entity->VelocityMovement = MyPlayer->VelocityMovement;
        entity->VelocityPower = -MyPlayer->VelocityPower/2;

        // increase velocity and mark enemy as attacked
        MyPlayer->VelocityPower += MyPlayer->VelocityPower / (amount/200);
        DashedEnemies.push_back(std::weak_ptr(entity));
    }
}

void PlayerLogicProcessor::DashAttacking()
{
    auto MyPlayer = Owner.lock();
    if (MyPlayer->VelocityPower > 0 && !MyPlayer->Dodging) {
        // get enemies list
        std::vector<shared_ptr<Entity>>* array = &MyPlayer->game->MainEntityManager.Entities[EnemyType];
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
}

void PlayerLogicProcessor::DashLogic()
{
    auto MyPlayer = Owner.lock();
    // update dash cooldownm
    if (DashCooldown > 0)
        DashCooldown -= MyPlayer->game->GetGameDeltaTime();

    if (DashCooldown <= 0 && IsKeyDown(KEY_LEFT_SHIFT)) {
        if (!MyPlayer->IsPreparingForDash) {
            DashTimeStart = MyPlayer->game->GetGameTime();
            PlayerDashLineThickness = 10;
        }
        MyPlayer->IsPreparingForDash = true;
    }
    if (MyPlayer->IsPreparingForDash && !IsKeyDown(KEY_LEFT_SHIFT)) {
        MyPlayer->IsPreparingForDash = false;
    }
    if (MyPlayer->game->MainPlayer->IsPreparingForDash || MyPlayer->game->MainPlayer->MainWeaponsSystem.TimeStartedReloading != -1) {
        MyPlayer->game->MainCameraManager.QuickZoom(1.25f, 0.1f);
        if (MyPlayer->game->MainPlayer->IsPreparingForDash)
        {
            if (static_cast<float>(MyPlayer->game->GetGameTime() - DashTimeStart) / 1.1f > 0.8f)
                PlayerDashLineThickness = Lerp(PlayerDashLineThickness, 20, 2 * MyPlayer->game->GetGameDeltaTime());
            float alpha = static_cast<float>(MyPlayer->game->GetGameTime() - DashTimeStart) / 1.1f;
            Vector2 Target = GetScreenToWorld2D(GetMousePosition(), MyPlayer->game->MainCameraManager.RaylibCamera);
            float cx = MyPlayer->BoundingBox.x + MyPlayer->BoundingBox.width / 2;
            float cy = MyPlayer->BoundingBox.y + MyPlayer->BoundingBox.height / 2;
            float FinalAngle = (atan2(cy - Target.y, cx - Target.x) * RAD2DEG);
            Texture2D& anim_tex = MyPlayer->game->MainResourceManager.Textures["arrow"];
            float width = anim_tex.width;
            float height = anim_tex.height;
            BeginShaderMode(MyPlayer->game->MainResourceManager.Shaders["dash_arrow"]);
            float t = static_cast<float>(MyPlayer->game->GetGameTime() - DashTimeStart);
            SetShaderValue(MyPlayer->game->MainResourceManager.Shaders["dash_arrow"],
                MyPlayer->ShaderUniformLoc,
                &t,
                SHADER_ATTRIB_FLOAT);
            float size = 0.375f;
            DrawTexturePro(anim_tex, Rectangle(0, 0, width, height *6),
                           Rectangle(MyPlayer->BoundingBox.x + MyPlayer->BoundingBox.width/2 - cosf((FinalAngle) * DEG2RAD)*10,
                               MyPlayer->BoundingBox.y +MyPlayer->BoundingBox.height/2 - sinf((FinalAngle) * DEG2RAD)*10, width*size,
                                     height * 6 * size), Vector2(width * size * 0.5, height*6*size), FinalAngle-90, ColorAlpha(WHITE, alpha));
            EndShaderMode();
        }
    }
    if ((IsMouseButtonDown(1) || IsMouseButtonDown(0)) && MyPlayer->IsPreparingForDash && MyPlayer->Health > 0 && MyPlayer->game->GetGameTime() - DashTimeStart >= 0.35f) {
        DashCooldown = MyPlayer->game->LevelData[MyPlayer->game->CurrentLevelName]["player"]["dash_base_cooldown"].get<float>() + (2.2f - min(static_cast<float>(MyPlayer->game->GetGameTime() - DashTimeStart), 1.1f) * 2);
        DashedEnemies.clear();
        MyPlayer->VelocityMovement = Vector2Subtract(GetScreenToWorld2D(GetMousePosition(), MyPlayer->game->MainCameraManager.RaylibCamera), {MyPlayer->BoundingBox.x+MyPlayer->BoundingBox.width/2, MyPlayer->BoundingBox.y+MyPlayer->BoundingBox.height/2});
        MyPlayer->VelocityPower = MyPlayer->game->LevelData[MyPlayer->game->CurrentLevelName]["player"]["dash_base_power"].get<float>() * max(min(static_cast<float>(MyPlayer->game->GetGameTime() - DashTimeStart), 1.1f), 0.45f);
        MyPlayer->VelocityPower /= min(max((MyPlayer->Health / MyPlayer->MaxHealth)-2.0f, 1.0f), 1.25f);
        MyPlayer->VelocityPower *= MyPlayer->game->LevelData[MyPlayer->game->CurrentLevelName]["player"]["dash_power_multiplier"].get<float>();
        MyPlayer->game->MainSoundManager.PlaySoundM("dash");
        MyPlayer->PlayerFrozenTimer = MyPlayer->game->LevelData[MyPlayer->game->CurrentLevelName]["player"]["dash_frozen_multiplier"].get<float>() *
            min(max((MyPlayer->VelocityPower / MyPlayer->game->LevelData[MyPlayer->game->CurrentLevelName]["player"]["dash_base_power"].get<float>()), 0.35f), 1.1f);
        if (!MyPlayer->isInvincible)
        {
            MyPlayer->ToggleInvincibility();
            MyPlayer->InvincibilityResetTimer = MyPlayer->game->LevelData[MyPlayer->game->CurrentLevelName]["player"]["dash_iframe_time"].get<float>();
        }
        if (IsMouseButtonDown(0)) {

            MyPlayer->Dodging = true;
            MyPlayer->InvincibilityResetTimer = MyPlayer->game->LevelData[MyPlayer->game->CurrentLevelName]["player"]["dodge_iframe_time"].get<float>();
            DashCooldown = MyPlayer->game->LevelData[MyPlayer->game->CurrentLevelName]["player"]["dodge_cooldown"].get<float>();
        }
        MyPlayer->IsPreparingForDash = false;
    }

    // display dashing bar
    int w = 56;
    int h = 15;
    float a = 0;
    if (MyPlayer->IsPreparingForDash)
        a = Lerp(0, 0.6f, min( (float)(MyPlayer->game->GetGameTime() - DashTimeStart) / 0.2f, 1.0f ));

    DrawRectangle((int)(MyPlayer->BoundingBox.x + (MyPlayer->BoundingBox.width / 2) - (w/2)),
        (int)(MyPlayer->BoundingBox.y + MyPlayer->BoundingBox.width + 10),
        w, h, ColorAlpha(BLACK, a));

    DrawRectangle(
        (int)(MyPlayer->BoundingBox.x + (MyPlayer->BoundingBox.width / 2) - (w/2))+5,
        (int)(MyPlayer->BoundingBox.y + MyPlayer->BoundingBox.width + 10)+5,
        (!MyPlayer->IsPreparingForDash ? 0 : min(static_cast<float>(MyPlayer->game->GetGameTime() - DashTimeStart) / 1.1f, 1.0f))*(w-10),
        h-10,
        ColorAlpha(WHITE, a*1.25f));

    // Health bar
    DrawRectangle((int)(MyPlayer->BoundingBox.x-20-h),
       (int)(MyPlayer->BoundingBox.y + (MyPlayer->BoundingBox.height / 2) - (w/2)),
       h, w,
       ColorAlpha(BLACK, 0.4f));

    DrawRectangle((int)(MyPlayer->BoundingBox.x-20-h)+5,
       (int)(MyPlayer->BoundingBox.y + (MyPlayer->BoundingBox.height / 2) - (w/2))+5,
       h - 10, min(MyPlayer->Health / 100, 1.0f) * (w - 10.0f),
       ColorAlpha(GREEN, (MyPlayer->HealthConcern ? (MyPlayer->WarningSign ? 0 : 0.8f) : 0.8f) ));

    // Ammo bar
    if (MyPlayer->MainWeaponsSystem.CurrentWeapon != nullptr && MyPlayer->MainWeaponsSystem.CurrentWeapon->Ammo > 0)
    {
        DrawRectangle((int)(MyPlayer->BoundingBox.x+MyPlayer->BoundingBox.height+20),
       (int)(MyPlayer->BoundingBox.y + (MyPlayer->BoundingBox.height / 2) - (w/2)),
       h, w,
       ColorAlpha(BLACK, 0.4f));

        DrawRectangle((int)(MyPlayer->BoundingBox.x+MyPlayer->BoundingBox.height+20)+5,
           (int)(MyPlayer->BoundingBox.y + (MyPlayer->BoundingBox.height / 2) - (w/2))+5,
           h - 10, ((float)MyPlayer->MainWeaponsSystem.WeaponAmmo[MyPlayer->MainWeaponsSystem.CurrentWeaponIndex] / MyPlayer->MainWeaponsSystem.CurrentWeapon->Ammo) * (w - 10.0f),
           ColorAlpha(YELLOW, 0.8f ));
    }
}

void PlayerLogicProcessor::DisplayDamageNotifs()
{
    auto MyPlayer = Owner.lock();
    std::erase_if(DamageNotifs, [this, MyPlayer](Vector3 e)
    {
        return (float)MyPlayer->game->GetGameTime() - e.z >= 1.5f;
    });
    for (Vector3 notif : DamageNotifs)
    {
        float time = (float)MyPlayer->game->GetGameTime() - notif.z;
        Vector2 m = Vector2Multiply(Vector2Normalize(Vector2Subtract({MyPlayer->BoundingBox.x + MyPlayer->BoundingBox.width/2, MyPlayer->BoundingBox.y + MyPlayer->BoundingBox.height/2}, {notif.x, notif.y})),
            {200, 200});
        float angle = 180 - Vector2LineAngle({0,0},m) * RAD2DEG;
        float trans = 1.0f;
        if (time <= 0.5f)
            trans = time / 0.5f;
        if (time >= 1.0f)
            trans = (time - 1.0f) / 0.5f;
        DrawCircleSector(Vector2Subtract({MyPlayer->BoundingBox.x + MyPlayer->BoundingBox.width/2, MyPlayer->BoundingBox.y + MyPlayer->BoundingBox.height/2}, m), 45, (180+angle)-50, (180+angle)+50, 10, ColorAlpha(WHITE, trans/2));
    }
}
