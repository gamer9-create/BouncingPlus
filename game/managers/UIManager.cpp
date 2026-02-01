//
// Created by lalit on 9/28/2025.
//

#include "UIManager.h"
#include <algorithm>
#include <iostream>
#include <raymath.h>

#include "../Game.h"

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
    this->GameWinScreen = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    this->LastHealth = 0;
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

    if (GameWinScreen.texture.width != GetScreenWidth() || GameWinScreen.texture.height != GetScreenHeight()) {
        UnloadRenderTexture(GameWinScreen);
        GameWinScreen = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
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
                WeaponSlotOffset += 240 * game->GetGameDeltaTime();
                WeaponSlotSize += 100 * game->GetGameDeltaTime();
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
        WeaponSlotOffset -= 240 * game->GetGameDeltaTime();
        WeaponSlotSize -= 100 * game->GetGameDeltaTime();
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
    HealthBarAnimRot = Lerp(HealthBarAnimRot, 0, 3.5f * game->GetGameDeltaTime());
    float limit = 25;
    ft_size = Lerp(ft_size, 92, 3.5f*game->GetGameDeltaTime());
    float PlrHealth = (!game->MainPlayer->isInvincible ? game->MainPlayer->Health : game->MainPlayer->PrevHealthBeforeDodge);
    if (LastHealth != PlrHealth)
    {
        HealthBarAnimRot = limit * ((PlrHealth-LastHealth) / abs(PlrHealth-LastHealth));
        game->MainCameraManager.QuickZoom(PlrHealth - LastHealth > 0 ? 0.9f : 1.1f, 0.1f);
    }
    if (LastHealth != PlrHealth && PlrHealth > LastHealth) {
        ft_size = 135;
    }
    float size = MeasureText((std::to_string((int)round(PlrHealth))+"%").c_str(), ft_size);
    DrawRectangle((WeaponUITexture.texture.width / 2.0f - size / 2.0f)-margin,(WeaponUITexture.texture.height / 2.0f - ft_size/2)-margin,size+(margin*2),ft_size+(margin*2),
        ColorLerp(ColorLerp(ColorAlpha((game->MainPlayer->isInvincible ? WHITE : BLACK), alpha), RED, max(-HealthBarAnimRot,0.01f) / limit),GREEN,
            (ft_size - 92)/43));
    DrawTextPro(GetFontDefault(), (std::to_string((int)round(PlrHealth))+"%").c_str(),
        {WeaponUITexture.texture.width / 2.0f,
        WeaponUITexture.texture.height / 2.0f},
        {size/2, ft_size/2},
        HealthBarAnimRot,
        ft_size,
        10,
        GetHealthColor(game->MainPlayer->Health/game->MainPlayer->MaxHealth));
    LastHealth = PlrHealth;

    if (game->DebugDraw && game->MainPlayer->weaponsSystem.CurrentWeapon != nullptr)
        DrawText(("Weapon info " + to_string(game->MainPlayer->weaponsSystem.CurrentWeaponIndex) + " "
            + to_string(game->MainPlayer->weaponsSystem.AttackCooldowns[game->MainPlayer->weaponsSystem.CurrentWeaponIndex]) + " " +
            game->MainPlayer->weaponsSystem.Weapons[game->MainPlayer->weaponsSystem.CurrentWeaponIndex] + " " +
            to_string(game->MainPlayer->weaponsSystem.WeaponAmmo[game->MainPlayer->weaponsSystem.CurrentWeaponIndex])
            ).c_str(), 0, 0, 25, WHITE);

    EndTextureMode();

    DeathTextAnimRot = sin(GetTime()*2) * 6;

    BeginTextureMode(DeathScreen);
    ClearBackground(ColorAlpha(RED, 0.2f));
    EndTextureMode();

    BeginTextureMode(GameWinScreen);
    ClearBackground(ColorAlpha(GREEN, 0.2f));
    EndTextureMode();

    if (game->MainPlayer->ShouldDelete || game->MainPlayer->Health <= 0)
        DeathMenu();

    if (game->DebugDraw)
        DrawText(to_string(UITransparency).c_str(), 50, 250, 10, WHITE);

    /*
    *if (game->MainEntityManager.Entities[EnemyType].size() <= 0)
        GameWin();
    else
        DrawTextureRec(DeathScreen.texture, Rectangle(0, 0, DeathScreen.texture.width, -DeathScreen.texture.height), Vector2(0, GetScreenHeight() - DeathScreen.texture.height), ColorAlpha(WHITE, ((1-UITransparency)-0.5f)/0.5f));

     */

    DrawTextureRec(DeathScreen.texture, Rectangle(0, 0, DeathScreen.texture.width, -DeathScreen.texture.height), Vector2(0, GetScreenHeight() - DeathScreen.texture.height), ColorAlpha(WHITE, ((1-UITransparency)-0.5f)/0.5f));

    DrawTextureRec(WeaponUITexture.texture, Rectangle(0, 0, WeaponUITexture.texture.width, -WeaponUITexture.texture.height), Vector2(0, GetScreenHeight() - WeaponUITexture.texture.height), ColorAlpha(WHITE, UITransparency));

    if (game->MainPlayer->Health > 0 && UITransparency < 1.0f) {
        UITransparency += 3 * game->GetGameDeltaTime();
    } else if (UITransparency > 0) {
        UITransparency -= 2.2f*game->GetGameDeltaTime();
    }

    if (game->MainGameModeManager.LevelTimer > 0)
        DisplayTimer();

    if (game->Paused)
        PauseMenu();
}

void UIManager::DisplayTimer()
{
    int minutes = (int)(this
        ->game->MainGameModeManager.LevelTimer / 60);
    int seconds = (int)(this->game->MainGameModeManager.LevelTimer - (minutes * 60.0f));
    std::string txt = to_string(minutes) + ":" + to_string(seconds);
    if (seconds < 10)
        txt = to_string(minutes) + ":0" + to_string(seconds);
    int font_size = 50;
    int width = MeasureText(txt.c_str(), font_size);
    float x = GetScreenWidth() / 2 - width / 2;
    float y = 50 + font_size;
    Rectangle rec = {x - 10, y - 10, width + 20.0f, font_size + 20.0f};
    DrawRectangleRec({rec.x - 10, rec.y - 10, rec.width + 20, rec.height + 20}, ColorAlpha(BLACK, UITransparency * 0.75f));
    DrawText(txt.c_str(), x, y, font_size, ColorAlpha(WHITE, UITransparency));
    DrawRectangleLinesEx(rec, 5, ColorAlpha(WHITE, UITransparency));

    std::string o_txt = "TIME LEFT";
    if (game->MainGameModeManager.CurrentGameMode == "wave")
    {
        if (!game->MainGameModeManager.InWave)
            o_txt = "INTERMISSION TO WAVE " + to_string(game->MainGameModeManager.CurrentWave + 1);
        else
            o_txt = "SURVIVE WAVE " + to_string(game->MainGameModeManager.CurrentWave);
    }

    DrawText(o_txt.c_str(), rec.x + rec.width/2 - MeasureText(o_txt.c_str(), font_size / 1.5f)/2, rec.y - (font_size/1.5f) - 15, font_size / 1.5f, ColorAlpha(WHITE, UITransparency));
}

void UIManager::DeathMenu()
{
    BeginTextureMode(DeathScreen);

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

void UIManager::PauseMenu() {
    BeginTextureMode(PauseScreen);
    ClearBackground(ColorAlpha(BLACK, 0.35f));
    DrawRectangle(PauseScreen.texture.width/2 - 225, PauseScreen.texture.height/2-175,450, 350,ColorAlpha(WHITE,0.85f));
    game->Paused = !button({(float)PauseScreen.texture.width/2,(float)PauseScreen.texture.height/2-100}, "RESUME");
    game->ShouldReturn = button({(float)PauseScreen.texture.width/2,(float)PauseScreen.texture.height/2+100}, "QUIT");
    EndTextureMode();
    BeginBlendMode(BLEND_ALPHA_PREMULTIPLY);
    DrawTextureRec(PauseScreen.texture, Rectangle(0, 0, PauseScreen.texture.width, -PauseScreen.texture.height), Vector2(0, GetScreenHeight() - PauseScreen.texture.height), WHITE);
    EndBlendMode();

}

void UIManager::GameWin()
{
    BeginTextureMode(GameWinScreen);
    ClearBackground(ColorAlpha(GREEN, 0.2f));

    DrawTextPro(GetFontDefault(), "You won!", {GetScreenWidth()/2.0f, 250.0f}, {MeasureTextEx(GetFontDefault(), "You won!", 150, 10.0f).x/2.0f, 50.0f}, DeathTextAnimRot, 150, 10, ColorBrightness(GREEN, -0.3f));

    int ey = GetScreenHeight()-400;
    int es = 50;
    std::string txt = "YOU KILLED " + to_string(game->MainPlayer->Kills) + " ENEMIES";
    if (game->MainPlayer->Kills == 1)
        txt = "YOU KILLED 1 ENEMY";
    if (game->MainPlayer->Kills == 0)
        txt = "YOU DID NOT KILL ANY ENEMIES";
    std::string txt_2 = "PRESS E TO RETURN TO TITLE SCREEN";
    float size = MeasureText(txt.c_str(), es);
    float size2 = MeasureText(txt_2.c_str(), es);

    DrawText(txt.c_str(), GetScreenWidth()/2 - size/2, ey-DeathTextAnimRot, es, ColorBrightness(RED, -0.1f));
    DrawText(txt_2.c_str(), GetScreenWidth()/2 - size2/2, ey+es-DeathTextAnimRot, es, ColorBrightness(RED, -0.1f));

    EndTextureMode();
    DrawTextureRec(GameWinScreen.texture, Rectangle(0, 0, GameWinScreen.texture.width, -GameWinScreen.texture.height), Vector2(0, GetScreenHeight() - GameWinScreen.texture.height), ColorAlpha(WHITE, ((1-UITransparency)-0.5f)/0.5f));
}

void UIManager::Quit() {
    UnloadRenderTexture(WeaponUITexture);
    UnloadRenderTexture(PauseScreen);
    UnloadTexture(button_img);
    UnloadRenderTexture(GameWinScreen);
    UnloadRenderTexture(DeathScreen);
}
