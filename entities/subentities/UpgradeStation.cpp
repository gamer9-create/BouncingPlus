//
// Created by lalit on 10/28/2025.
//

#include "UpgradeStation.h"

#include <iostream>
#include <raymath.h>

#include "../../game/Game.h"

double to_positive_angle(double angle)
{
    angle = fmod(angle, 360);
    if (angle < 0) angle += 360;
    return angle;
}

UpgradeStation::UpgradeStation() {
}

UpgradeStation::UpgradeStation(Game &game, float bbox_x, float bbox_y) :Entity(game.Textures["nut"], {bbox_x-7, bbox_y-7, 15, 15}, 0, game) {
    isAnimating=false;
    GoalAngle=Rotation;
    CooldownTimer = 0;
    SecondArmRot = 0;
    Ani1Timer = 0;
}

void UpgradeStation::Render() {
    for (int i = 0; i < 3; i ++) {
        DrawCircle(BoundingBox.x+7,
            BoundingBox.y+7,
            36-i*2, ColorLerp(GRAY, WHITE, 0.1f * i));
    }
    float w = 75;
    Vector2 sv = {cos(Rotation * (2 * PI / 360)),sin(Rotation * (2 * PI / 360))};
    Vector2 uv = {cos((Rotation+SecondArmRot) * (2 * PI / 360)),sin((Rotation+SecondArmRot) * (2 * PI / 360))};
    Vector2 v = Vector2Multiply(sv, {-w/2, -w/2});
    Vector2 u = Vector2Multiply(sv, {-w, -w});
    uv = Vector2Multiply(uv, {-w, -w});
    v = Vector2Add({BoundingBox.x+BoundingBox.width/2,BoundingBox.y+BoundingBox.height/2}, v);
    u = Vector2Add({BoundingBox.x+BoundingBox.width/2,BoundingBox.y+BoundingBox.height/2}, u);
    s = Vector2Add(u, uv);
    DrawRectanglePro({v.x, v.y, w, 36}, {w/2, 18}, Rotation, GRAY);
    DrawRectanglePro({u.x, u.y, w, 36}, {w, 18}, Rotation+SecondArmRot, GRAY);
    Vector2 PlayerPos = {game->MainPlayer->BoundingBox.x+game->MainPlayer->BoundingBox.width/2,game->MainPlayer->BoundingBox.y+game->MainPlayer->BoundingBox.height/2};
    float F = 180-(Vector2LineAngle(s, PlayerPos)*RAD2DEG);
    if (!isAnimating || abs(Rotation-GoalAngle) <= 10) {
        float A = F - Rotation;
        if (Vector2Distance(s, PlayerPos) <= 50)
            A = -180;
        SecondArmRot = Lerp(SecondArmRot, A, 2 * GetFrameTime());
    }
    DrawTexturePro(game->Textures["arm_tip"],{0,0,36,36},{s.x,s.y,36,36},{18,36},Rotation+SecondArmRot-90,GRAY);
    for (int i = 0; i < 3; i ++) {
        DrawCircle(u.x,
            u.y,
            18-i*2, ColorLerp(GRAY, WHITE, 0.1f * i));
    }
}

void UpgradeStation::PhysicsUpdate(float dt) {
    Entity::PhysicsUpdate(dt);
    Vector2 PlayerPos = {game->MainPlayer->BoundingBox.x+game->MainPlayer->BoundingBox.width/2,game->MainPlayer->BoundingBox.y+game->MainPlayer->BoundingBox.height/2};
    if (Vector2Distance(s, PlayerPos) <= 50) {
        PlayerPos = s;
        game->MainPlayer->BoundingBox.x = PlayerPos.x - game->MainPlayer->BoundingBox.width/2;
        game->MainPlayer->BoundingBox.y = PlayerPos.y - game->MainPlayer->BoundingBox.height/2;
        game->MainPlayer->weaponsSystem.Unequip();
        if (!game->MainPlayer->isInvincible)
            game->MainPlayer->ToggleInvincibility();
        if (Vector2Distance({BoundingBox.x, BoundingBox.y}, PlayerPos) <= 10) {
            game->UpgradeUI = true;
        }
    }
}

UpgradeStation::~UpgradeStation() {
}

void UpgradeStation::AnimateTowards(float Angle) {
    if (!isAnimating) {
        GoalAngle = to_positive_angle(Angle);
        isAnimating = true;
        CooldownTimer= 0;
        Ani1Timer=0;
    }
}

void UpgradeStation::Update() {
    if (isAnimating) {
        if (abs(Rotation-GoalAngle) <= 10) {
            CooldownTimer += GetFrameTime();
            if (CooldownTimer >= 4)
                isAnimating = false;
        } else {
            Ani1Timer += GetFrameTime();
            if (!(Ani1Timer >= 0.4f && Ani1Timer <= 0.45f))
                Rotation -= 0.5f * GetFrameTime() * (Rotation - GoalAngle > 0) ? 1 : -1;
        }
    }

    //Rotation = 180-(Vector2LineAngle({BoundingBox.x+BoundingBox.width/2,BoundingBox.y+BoundingBox.height/2}, {game->MainPlayer->BoundingBox.x+game->MainPlayer->BoundingBox.width/2,game->MainPlayer->BoundingBox.y+game->MainPlayer->BoundingBox.height/2})*RAD2DEG);
    AnimateTowards(180-(Vector2LineAngle({BoundingBox.x+BoundingBox.width/2,BoundingBox.y+BoundingBox.height/2}, {game->MainPlayer->BoundingBox.x+game->MainPlayer->BoundingBox.width/2,game->MainPlayer->BoundingBox.y+game->MainPlayer->BoundingBox.height/2})*RAD2DEG)+
        GetRandomValue(-35, 35));
    Entity::Update();
    Render();
}
