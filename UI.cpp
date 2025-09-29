//
// Created by lalit on 9/28/2025.
//

#include "UI.h"
#include "Game.h"

UI::UI(Game &game) {
    this->game = &game;
    this->WeaponUITexture = LoadRenderTexture(250, 250);
}

UI::UI() {
}

void UI::WeaponUI() {
    //DrawRectangleRounded(Rectangle(25, GetScreenHeight()-175, 250, 150), 0.15f, 1, RAYWHITE);
    BeginTextureMode(WeaponUITexture);
    for (int i = 0; i < 3; i++) {
        std::string name = game->MainPlayer->weaponsSystem.Weapons[i];
        if (name.empty())
            name = "Empty";
        DrawLineEx(Vector2(50, (35.0f * i)-2.0f),
                Vector2((50 + MeasureText(name.c_str(), 20)), (35.0f * i)-2.0f), 4,
                       BLACK);
        DrawText(name.c_str(), 50, (35 * i), 20, BLACK);
    }
    EndTextureMode();
    DrawTextureRec(WeaponUITexture.texture, Rectangle(0, 0, WeaponUITexture.texture.width, -WeaponUITexture.texture.height), Vector2(0, GetScreenHeight() - WeaponUITexture.texture.height/2), RAYWHITE);
}
