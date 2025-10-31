//
// Created by lalit on 9/28/2025.
//

#include "UIManager.h"
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

UIManager::UIManager(Game &game) {
    this->game = &game;
    this->WeaponUITexture = LoadRenderTexture(GetScreenWidth(), 250);
    this->DeathScreen = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    this->PauseScreen = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    button_img = LoadTexture("assets/ui/button.png");
}

bool UIManager::button(Vector2 pos, std::string text) {
    Rectangle rectangle = {0,0, (float)MeasureText(text.c_str(), 50)+25, (float)button_img.height};
    rectangle.x=pos.x - rectangle.width/2;
    rectangle.y=pos.y - rectangle.height/2;

    DrawTexturePro(button_img, {0,0,8,56}, {rectangle.x,rectangle.y,7,56}, {0,0}, 0, WHITE);
    DrawTexturePro(button_img, {143,0,7,56}, {rectangle.x+rectangle.width-7,rectangle.y,7,56}, {0,0}, 0, WHITE);
    DrawTexturePro(button_img, {8,0,135,56}, {rectangle.x+7,rectangle.y,rectangle.width-14,56}, {0,0}, 0, WHITE);

    DrawText(text.c_str(), rectangle.x + rectangle.width/2 - MeasureText(text.c_str(), 50)/2, rectangle.y + rectangle.height/2 - 25, 50, WHITE);

    if (CheckCollisionPointRec(GetMousePosition(), rectangle)) {
        DrawRectangleLinesEx(rectangle, 4, WHITE);
        return IsMouseButtonPressed(0);
    }

    return false;

}

UIManager::UIManager() {
}

void UIManager::GameUI() {

    if (WeaponUITexture.texture.width != GetScreenWidth()) {
        UnloadRenderTexture(WeaponUITexture);
        WeaponUITexture = LoadRenderTexture(GetScreenWidth(), WeaponUITexture.texture.height);
    }

    if (DeathScreen.texture.width != GetScreenWidth() || DeathScreen.texture.height != GetScreenHeight()) {
        UnloadRenderTexture(DeathScreen);
        DeathScreen = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    }

    if (PauseScreen.texture.width != GetScreenWidth() || PauseScreen.texture.height != GetScreenHeight()) {
        UnloadRenderTexture(PauseScreen);
        PauseScreen = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    }

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

    // cooldown meter
    if (game->MainPlayer != nullptr && game->MainPlayer->weaponsSystem.CurrentWeapon != nullptr) {
        Rectangle r = {lowest_x + highest_width + margin + 50, lowest_y-margin, 65, highest_height+margin*2};
        DrawRectangle(r.x,r.y,r.width,r.height, ColorAlpha(BLACK, alpha));
        float percent = min(game->MainPlayer->weaponsSystem.AttackCooldowns[game->MainPlayer->weaponsSystem.CurrentWeaponIndex]/game->MainPlayer->weaponsSystem.CurrentWeapon->Cooldown, 1.0f);
        Color g = WHITE;
        if (percent == 1.0f)
            g = GREEN;
        DrawRectangle(r.x + 10, r.y + 10 + ((r.height-20) * (1-percent)), r.width-20, (r.height-20) * percent, g);

        //ammo meter
        if (game->MainPlayer->weaponsSystem.CurrentWeapon->Ammo > 0)
        {
            DrawRectangle(r.x + r.width + margin, WeaponUITexture.texture.height - (100+margin), 125, 100, ColorAlpha(BLACK, alpha));
            float s2 = 70;
            float s = MeasureText(to_string(game->MainPlayer->weaponsSystem.WeaponAmmo[game->MainPlayer->weaponsSystem.CurrentWeaponIndex]).c_str(), s2);
            DrawText(to_string(game->MainPlayer->weaponsSystem.WeaponAmmo[game->MainPlayer->weaponsSystem.CurrentWeaponIndex]).c_str(), (int)(r.x + r.width + margin + 125/2 - s/2),
            WeaponUITexture.texture.height - (100+margin) + 10, s2, WHITE);

            float s3 = 20;
            float s4 = MeasureText("Ammo", s3);
            DrawText("Ammo", (int)(r.x + r.width + margin + 125/2 - s4/2),
            WeaponUITexture.texture.height - (100+margin) + 100 - s3 - 10, s3, WHITE
                );
        }
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

    // kill counter
    DrawRectangle(GetScreenWidth()-(125+margin), WeaponUITexture.texture.height - (100+margin), 125, 100, ColorAlpha(BLACK, alpha));
    float s2 = 70;
    float s = MeasureText(to_string(game->MainPlayer->Kills).c_str(), s2);
    DrawText(to_string(game->MainPlayer->Kills).c_str(), (int)(GetScreenWidth()-(125+margin) + 125/2 - s/2),
    WeaponUITexture.texture.height - (100+margin) + 10, s2, WHITE);

    float s3 = 20;
    float s4 = MeasureText("Kills", s3);
    DrawText("Kills", (int)(GetScreenWidth()-(125+margin) + 125/2 - s4/2),
    WeaponUITexture.texture.height - (100+margin) + 100 - s3 - 10, s3, WHITE
        );

    // health meter
    float PlrHealth = (game->MainPlayer->DodgeHealthResetTimer <= 0 ? game->MainPlayer->Health : game->MainPlayer->PrevHealthBeforeDodge);
    float size = MeasureText((std::to_string((int)round(PlrHealth))+"%").c_str(), 92);
    DrawRectangle((WeaponUITexture.texture.width / 2.0f - size / 2.0f)-margin,(WeaponUITexture.texture.height / 2.0f - 46)-margin,size+(margin*2),92+(margin*2),ColorAlpha((game->MainPlayer->DodgeHealthResetTimer <= 0 ? BLACK : WHITE), alpha));
    DrawText((std::to_string((int)round(PlrHealth))+"%").c_str(), WeaponUITexture.texture.width / 2.0f - size / 2.0f, WeaponUITexture.texture.height / 2.0f - 46, 92, GetHealthColor(game->MainPlayer->Health/game->MainPlayer->MaxHealth));

    if (game->DebugDraw && game->MainPlayer->weaponsSystem.CurrentWeapon != nullptr)
        DrawText(("Weapon info " + to_string(game->MainPlayer->weaponsSystem.CurrentWeaponIndex) + " "
            + to_string(game->MainPlayer->weaponsSystem.AttackCooldowns[game->MainPlayer->weaponsSystem.CurrentWeaponIndex]) + " " +
            game->MainPlayer->weaponsSystem.Weapons[game->MainPlayer->weaponsSystem.CurrentWeaponIndex] + " " +
            to_string(game->MainPlayer->weaponsSystem.WeaponAmmo[game->MainPlayer->weaponsSystem.CurrentWeaponIndex])
            ).c_str(), 0, 0, 25, WHITE);

    EndTextureMode();

    if (game->MainPlayer->IsDashing || game->MainPlayer->weaponsSystem.TimeStartedReloading != -1) {
        game->MainCameraManager.QuickZoom(1.25f, 1);
        if (game->MainPlayer->IsDashing)
        {
            Vector2 ss = {game->MainPlayer->BoundingBox.x + game->MainPlayer->BoundingBox.width/2 - game->MainCameraManager.CameraPosition.x,
            game->MainPlayer->BoundingBox.y + game->MainPlayer->BoundingBox.height/2 - game->MainCameraManager.CameraPosition.y};
            DrawLineEx(ss, {(float)GetMouseX(), (float)GetMouseY()}, 10, WHITE);
        }
    }

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

        DrawText(txt.c_str(), GetScreenWidth()/2 - size/2, ey-DeathTextAnimRot, es, ColorBrightness(RED, -0.1f));
        DrawText(txt_2.c_str(), GetScreenWidth()/2 - size2/2, ey+es-DeathTextAnimRot, es, ColorBrightness(RED, -0.1f));

        EndTextureMode();
    }

    //if (game->DebugDraw)
    //    DrawText(to_string(UIManagerTransparency).c_str(), 50, 250, 10, WHITE);
    DrawTextureRec(WeaponUITexture.texture, Rectangle(0, 0, WeaponUITexture.texture.width, -WeaponUITexture.texture.height), Vector2(0, GetScreenHeight() - WeaponUITexture.texture.height), ColorAlpha(WHITE, UITransparency));
    DrawTextureRec(DeathScreen.texture, Rectangle(0, 0, DeathScreen.texture.width, -DeathScreen.texture.height), Vector2(0, GetScreenHeight() - DeathScreen.texture.height), ColorAlpha(WHITE, ((1-UITransparency)-0.5f)/0.5f));
    if (game->MainPlayer->Health > 0 && UITransparency < 1.0f) {
        UITransparency += 3 * GetFrameTime();
    } else if (UITransparency > 0) {
        UITransparency -= 2.2f*GetFrameTime();
    }

    if (game->Paused)
        PauseMenu();
}

void UIManager::PauseMenu() {
    BeginTextureMode(PauseScreen);
    ClearBackground(ColorAlpha(BLACK, 0.35f));
    DrawRectangle(PauseScreen.texture.width/2 - 225, PauseScreen.texture.height/2-175,450, 350,ColorAlpha(WHITE,0.85f));
    game->Paused =!button({(float)PauseScreen.texture.width/2,(float)PauseScreen.texture.height/2-100}, "RESUME");
    EndTextureMode();
    BeginBlendMode(BLEND_ALPHA_PREMULTIPLY);
    DrawTextureRec(PauseScreen.texture, Rectangle(0, 0, PauseScreen.texture.width, -PauseScreen.texture.height), Vector2(0, GetScreenHeight() - PauseScreen.texture.height), WHITE);
    EndBlendMode();
}

void UIManager::Quit() {
    UnloadRenderTexture(WeaponUITexture);
    UnloadRenderTexture(PauseScreen);
    UnloadTexture(button_img);
    UnloadRenderTexture(DeathScreen);
}
