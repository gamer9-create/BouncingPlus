//
// Created by lalit on 9/28/2025.
//

#include "UI.h"
#include <algorithm>
#include "Game.h"

Color GetHealthColor(float Percent) {
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
    this->WeaponUITexture = LoadRenderTexture(1280, 125);
    this->HealthBarTexture = LoadTexture("assets/img/health_bar.png");
}

UI::UI() {
}

void UI::WeaponUI() {
    //DrawRectangleRounded(Rectangle(25, GetScreenHeight()-175, 250, 150), 0.15f, 1, RAYWHITE);
    BeginTextureMode(WeaponUITexture);
    ClearBackground(BLANK);
    float Prev = 0;
    bool Found = false;
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

    float size = MeasureText(std::to_string((int)round(game->MainPlayer->Health)).c_str(), 92);
    DrawText(std::to_string((int)round(game->MainPlayer->Health)).c_str(), WeaponUITexture.texture.width / 2.0f - size / 2.0f, WeaponUITexture.texture.height / 2.0f - 46, 92, GetHealthColor(game->MainPlayer->Health/game->MainPlayer->MaxHealth));

    EndTextureMode();
    DrawTextureRec(WeaponUITexture.texture, Rectangle(0, 0, WeaponUITexture.texture.width, -WeaponUITexture.texture.height), Vector2(0, GetScreenHeight() - WeaponUITexture.texture.height), RAYWHITE);
}

void UI::Quit() {
    UnloadRenderTexture(WeaponUITexture);
    UnloadTexture(HealthBarTexture);
}