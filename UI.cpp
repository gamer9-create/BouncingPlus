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
    this->WeaponUITexture = LoadRenderTexture(GetScreenWidth(), 125);
    this->DeathScreen = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    this->HealthBarTexture = LoadTexture("assets/img/health_bar.png");
}

UI::UI() {
}

void UI::WeaponUI() {
    BeginTextureMode(WeaponUITexture);
    ClearBackground(BLANK);
    float Prev = 0;
    bool Found = false;

    float lowest_x = 50.0f;
    float lowest_y = 0.0f;
    float highest_width = -1;
    float highest_height = -1;

    float tPrev = 0;
    for (int i = 0; i < 3; i ++ )
    {
        float offset = 0;
        float size = 0;
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
        if (MeasureText(name.c_str(), 20+size) > highest_width)
            highest_width = MeasureText(name.c_str(), 20+size)+offset;
        tPrev += 35 + size;
    }

    int margin = 15;
    DrawRectangle(lowest_x-margin,lowest_y-margin,highest_width+margin,highest_height+margin,ColorAlpha(BLACK, 0.5f));

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

    float size = MeasureText((std::to_string((int)round(game->MainPlayer->Health))+"%").c_str(), 92);
    DrawText((std::to_string((int)round(game->MainPlayer->Health))+"%").c_str(), WeaponUITexture.texture.width / 2.0f - size / 2.0f, WeaponUITexture.texture.height / 2.0f - 46, 92, GetHealthColor(game->MainPlayer->Health/game->MainPlayer->MaxHealth));

    EndTextureMode();

    DeathTextAnimRot = sin(GetTime()*2) * 6;

    if (game->MainPlayer->ShouldDelete) {
        BeginTextureMode(DeathScreen);

        ClearBackground(ColorAlpha(RED, 0.2f));

        DrawTextPro(GetFontDefault(), "You died!", {GetScreenWidth()/2.0f, 250.0f}, {MeasureTextEx(GetFontDefault(), "You died!", 100, 10.0f).x/2.0f, 50.0f}, DeathTextAnimRot, 100, 10, RED);

        const char* txt = ("YOU KILLED " + to_string(game->MainPlayer->Kills) + " ENEMIES").c_str();
        const char* txt_2 = std::string("PRESS E TO RESPAWN").c_str();
        float size = MeasureText(txt, 35);
        float size2 = MeasureText(txt_2, 35);
        DrawText(txt, GetScreenWidth()/2 - size/2, GetScreenHeight()-250, 35, RED);
        DrawText(txt_2, GetScreenWidth()/2 - size2/2, GetScreenHeight()-215, 35, RED);

        EndTextureMode();
    }

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