//
// Created by lalit on 9/28/2025.
//

#include "UI.h"
#include <algorithm>
#include <iostream>

#include "Game.h"

Color GetHealthColor(float Percent, float Armor) {
    if (Armor > 0)
        return BLUE;
    if (Percent >= 0.5f) {
        return ColorLerp(YELLOW, GREEN, (Percent - 0.5f) / 0.5f);
    }
    if (Percent < 0.5f) {
        return ColorLerp(RED, YELLOW, Percent / 0.5f);
    }
    return {0, 0, 0, 255};
}

UI::UI(Game &game) {
    this->game = &game;
    this->WeaponUITexture = LoadRenderTexture(GetScreenWidth(), 250);
    this->DeathScreen = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    this->HealthBarTexture = LoadTexture("assets/img/health_bar.png");
}

UI::UI() {
}

void UI::GameUI() {
    BeginTextureMode(WeaponUITexture);
    ClearBackground(BLANK);
    float Prev = 125;
    bool Found = false;

    float lowest_x = 50.0f;
    float lowest_y = 125.0f;
    float highest_width = -1;
    float highest_height = -1;

    float tPrev = 0;
    for (int i = 0; i < 3; i ++ )
    {
        float offset = 0;
        float size = 0;
        if (game->MainPlayer->weaponsSystem.CurrentWeaponIndex == i) {
            WeaponSlotIndex = i;
        }
        if (WeaponSlotIndex == i)
        {
            offset=WeaponSlotOffset;
            size=WeaponSlotSize;
        }
        std::string name = game->MainPlayer->weaponsSystem.Weapons[i];
        if (name.empty())
            name = "Empty";
        if (i == 2)
            highest_height = tPrev + 20 + size;
        if (MeasureText(name.c_str(), 20+size)+offset > highest_width)
            highest_width = MeasureText(name.c_str(), 20+size)+offset;
        tPrev += 35 + size;
    }

    int margin = 15;
    float alpha = 0.75f;
    DrawRectangle(lowest_x-margin,lowest_y-margin,highest_width+(margin*2),highest_height+(margin*2),ColorAlpha(BLACK, alpha));

    if (game->MainPlayer->weaponsSystem.CurrentWeapon != nullptr) {
        Rectangle r = {lowest_x + highest_width + margin + 50, lowest_y-margin, 65, highest_height+margin*2};
        DrawRectangle(r.x,r.y,r.width,r.height, ColorAlpha(BLACK, alpha));
        float percent = min(game->MainPlayer->weaponsSystem.AttackCooldowns[game->MainPlayer->weaponsSystem.CurrentWeaponIndex]/game->MainPlayer->weaponsSystem.CurrentWeapon->Cooldown, 1.0f);
        Color g = WHITE;
        if (percent == 1.0f)
            g = GREEN;
        DrawRectangle(r.x + 10, r.y + 10 + ((r.height-20) * (1-percent)), r.width-20, (r.height-20) * percent, g);
    }

    for (int i = 0; i < 3; i++) {
        std::string name = game->MainPlayer->weaponsSystem.Weapons[i];
        if (name.empty())
            name = "Empty";
        Color MainColor = RED;
        if (game->MainPlayer->weaponsSystem.CurrentWeaponIndex == i) {
            Found = true;
            WeaponSlotIndex = i;
        }
        float offset = 0;
        float size = 0;
        if (WeaponSlotIndex == i) {
            MainColor = GREEN;
            if (Found) {
                WeaponSlotOffset += 240 * GetFrameTime();
                WeaponSlotSize += 100 * GetFrameTime();
            }
            WeaponSlotOffset = clamp(WeaponSlotOffset, 0.0f, 60.0f);
            WeaponSlotSize = clamp(WeaponSlotSize, 0.0f, 15.0f);
            offset=WeaponSlotOffset;
            size=WeaponSlotSize;
        }
        if (MeasureText(name.c_str(), 20+size) > highest_width)
        {
            highest_width = MeasureText(name.c_str(), 20+size)+offset;
        }
        if (i == 2)
            highest_height = Prev + 20 + size;
        DrawLineEx(Vector2(50, Prev+2.0f),
                Vector2((50 + MeasureText(name.c_str(), 20+size)+offset), Prev+2.0f), 4,
                       MainColor);
        DrawText(name.c_str(), 50, Prev+4.0f, 20+size, MainColor);
        Prev += 20 + size + 15;
    }
    if (!Found) {
        WeaponSlotOffset -= 240 * GetFrameTime();
        WeaponSlotSize -= 100 * GetFrameTime();
        WeaponSlotOffset = clamp(WeaponSlotOffset, 0.0f, 60.0f);
        WeaponSlotSize = clamp(WeaponSlotSize, 0.0f, 15.0f);
        if (WeaponSlotOffset <= 0 && WeaponSlotSize <= 0)
            WeaponSlotIndex = -1;
    }

    if (game->MainPlayer->IsDashing)
        game->CameraZoom = 1.5f;
    else
        game->CameraZoom = 1.0f;

    float size = MeasureText((std::to_string((int)round(game->MainPlayer->Health))+"%").c_str(), 92);
    DrawRectangle((WeaponUITexture.texture.width / 2.0f - size / 2.0f)-margin,(WeaponUITexture.texture.height / 2.0f - 46)-margin,size+(margin*2),92+(margin*2),ColorAlpha(BLACK, alpha));
    DrawText((std::to_string((int)round(game->MainPlayer->Health))+"%").c_str(), WeaponUITexture.texture.width / 2.0f - size / 2.0f, WeaponUITexture.texture.height / 2.0f - 46, 92, GetHealthColor(game->MainPlayer->Health/game->MainPlayer->MaxHealth));

    EndTextureMode();

    DeathTextAnimRot = sin(GetTime()*2) * 6;

    if (game->MainPlayer->ShouldDelete || game->MainPlayer->Health <= 0) {
        BeginTextureMode(DeathScreen);

        ClearBackground(ColorAlpha(RED, 0.2f));

        DrawTextPro(GetFontDefault(), "You died!", {GetScreenWidth()/2.0f, 250.0f}, {MeasureTextEx(GetFontDefault(), "You died!", 100, 10.0f).x/2.0f, 50.0f}, DeathTextAnimRot, 100, 10, ColorBrightness(RED, -0.3f));

        int ey = GetScreenHeight()-400;
        int es = 50;
        std::string txt = "YOU KILLED " + to_string(game->MainPlayer->Kills) + " ENEMIES";
        if (game->MainPlayer->Kills == 1)
            txt = "YOU KILLED 1 ENEMY";
        if (game->MainPlayer->Kills == 0)
            txt = "YOU DID NOT KILL ANY ENEMIES";
        std::string txt_2 = "PRESS E TO RESPAWN";
        float size = MeasureText(txt.c_str(), es);
        float size2 = MeasureText(txt_2.c_str(), es);

        DrawText(txt.c_str(), GetScreenWidth()/2 - size/2, ey, es, ColorBrightness(RED, -0.1f));
        DrawText(txt_2.c_str(), GetScreenWidth()/2 - size2/2, ey+es, es, ColorBrightness(RED, -0.1f));

        EndTextureMode();
    }

    DrawText(to_string(UITransparency).c_str(), 50, 250, 10, WHITE);
    DrawTextureRec(WeaponUITexture.texture, Rectangle(0, 0, WeaponUITexture.texture.width, -WeaponUITexture.texture.height), Vector2(0, GetScreenHeight() - WeaponUITexture.texture.height), ColorAlpha(WHITE, UITransparency));
    DrawTextureRec(DeathScreen.texture, Rectangle(0, 0, DeathScreen.texture.width, -DeathScreen.texture.height), Vector2(0, GetScreenHeight() - DeathScreen.texture.height), ColorAlpha(WHITE, ((1-UITransparency)-0.5f)/0.5f));
    if (game->MainPlayer->Health > 0 && UITransparency < 1.0f) {
        UITransparency += 3 * GetFrameTime();
    } else if (UITransparency > 0) {
        UITransparency -= 2.2f*GetFrameTime();
    }
}

void UI::Quit() {
    UnloadRenderTexture(WeaponUITexture);
    UnloadTexture(HealthBarTexture);
}