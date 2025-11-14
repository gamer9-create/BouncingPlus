//
// Created by lalit on 10/16/2025.
//

#include "Menu.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <raymath.h>
using json = nlohmann::json;

using namespace std;

Menu::Menu(std::unordered_map<std::string,json> level_data)
{
    this->level_data = level_data;
    title_img = LoadTexture("assets/ui/title.png");
    button_img = LoadTexture("assets/ui/button.png");
    menu_img = LoadTexture("assets/ui/menu_img.png");
    miku_img = LoadTexture("assets/ui/miku.png");
    miku_sound = LoadSound("assets/ui/lovely_cavity.mp3");
    Reset();
    PlaySound(miku_sound);
}

void Menu::Reset()
{
    map = "";
    target_map = "";
    title_img_pos_y = -title_img.height;
    title_img_offset_y = 0;
    play_button_offset_y = -100;
    cam_x = 0;
    cam_x_targ = 0;
    off = 0;
    off2 = 0;
    off3 = 0;
    isStarting = true;
    BlackTransparency= 1.0f;
    miku_offset = 0.0f;
    menu_img_pos_y = GetRandomValue(0, menu_img.height);
    MovingToGame = false;
    mouse_pos = {0,0};
}

bool Menu::button(Rectangle rectangle, std::string text) {
    DrawTexture(button_img, rectangle.x-cam_x, rectangle.y, WHITE);
    DrawText(text.c_str(), rectangle.x-cam_x + button_img.width/2 - MeasureText(text.c_str(), 50)/2, rectangle.y + button_img.height/2 - 25, 50, WHITE);

    if (CheckCollisionPointRec(mouse_pos, rectangle)) {
        DrawRectangleLinesEx({rectangle.x-cam_x,rectangle.y,rectangle.width,rectangle.height}, 4, WHITE);
        return IsMouseButtonPressed(0);
    }
    return false;
}

void Menu::LevelSelect()
{
    float f = 400;

    Rectangle play_bbox = {(GetScreenWidth()/2.0f) - GetScreenWidth() - (int)(button_img.width/2.0f), (float)play_button_offset_y +f+off,150,56};
    if (button(play_bbox, "BACK")) {
        cam_x_targ=0;
    }
    DrawText("LEVEL SELECT", (int)(GetScreenWidth()/2.0f) - (int)(MeasureText("LEVEL SELECT", 50)/2.0f)-cam_x-GetScreenWidth(), 100+off2, 50, WHITE);
    int i = 0;
    for (auto& [name, data] : level_data)
    {
        Rectangle r = {-710.0f, (float)165 + (140 * i)-off3, (float)MeasureText(name.c_str(), 90)+20.0f, 110.0f};
        DrawRectangleRec({r.x-cam_x,r.y,r.width,r.height}, ColorAlpha(BLACK, 0.5f));
        Color c = RED;
        if (target_map==name) {
            c = GREEN;
            float comb = (off + off2 + off3)/3;
            DrawRectangle(-(GetScreenWidth()-125)-cam_x, 200-comb, GetScreenWidth()-900, GetScreenHeight()-450, ColorAlpha(BLACK, 0.5f));
            DrawText(name.c_str(), -(GetScreenWidth()-145)-cam_x, 220-comb, 50, WHITE);

            std::string description = to_string(data["description"]);

            DrawText(description.c_str(), -(GetScreenWidth()-145)-cam_x, 270-comb, 25, WHITE);
            DrawText(("Difficulty level: "+to_string(data["difficulty"])).c_str(), -(GetScreenWidth()-145)-cam_x, 200-comb + GetScreenHeight()-450 - 70 - 64, 25, ColorBrightness(RED, -1 + ((float)data["difficulty"] * 0.25f) ));
            Rectangle play_bbox = {(float)-(GetScreenWidth()-145), 200-comb + GetScreenHeight()-450 - 70,150,56};
            if (button(play_bbox, "PLAY")) {
                MovingToGame = true;
            }
        }
        DrawText(name.c_str(), -700 - cam_x, 175 + (140 * i)-off3, 90, c);
        if (CheckCollisionPointRec(mouse_pos, r)) {
            DrawRectangleLinesEx({r.x-cam_x,r.y,r.width,r.height}, 4, WHITE);
            target_map = IsMouseButtonPressed(0) ? (target_map != name ? name : "") : target_map;
        }
        i+=1;
    }
}

void Menu::Update() {
    if (!MovingToGame) {
        cam_x = Lerp(cam_x, cam_x_targ, 2 * GetFrameTime());
        mouse_pos = {(float)GetMouseX() + cam_x, (float)GetMouseY()};
    }

    off = -sin((GetTime()-20.0f) * 3.5f) * 15;
    off2 = -sin((GetTime()+20.0f) * 3.5f) * 15;
    off3 = -sin(GetTime() * 3.5f) * 15;

    title_img_pos_y = Lerp(title_img_pos_y, -100, 5 * GetFrameTime());
    title_img_offset_y = off;
    if (abs(title_img_pos_y) - 100 <= 10)
        play_button_offset_y = Lerp(play_button_offset_y, 550, 5 * GetFrameTime());

    menu_img_pos_y += GetFrameTime()*15;
    float zoom = 1.35f;
    DrawTexturePro(menu_img, {0, menu_img_pos_y, (float)GetScreenWidth(), (float)GetScreenHeight()},
        {GetScreenWidth()/2 - (GetScreenWidth() * zoom)/2 - cam_x/10, GetScreenHeight()/2 - (GetScreenHeight() * zoom)/2, (float)GetScreenWidth() * zoom, (float)GetScreenHeight() * zoom},
        {0,0},
        0,
        WHITE
        );

    DrawTexture(title_img, (int)(GetScreenWidth()/2.0f) - (int)(title_img.width/2.0f)-cam_x, (int)title_img_pos_y - (int)title_img_offset_y, WHITE);

    Rectangle play_bbox = {(GetScreenWidth()/2.0f) - (int)(button_img.width/2.0f), (float)play_button_offset_y +off3,150,56};
    if (button(play_bbox, "PLAY")) {
        cam_x_targ=-GetScreenWidth();
    }
    if (isStarting && BlackTransparency > 0)
        BlackTransparency -= 0.65f * GetFrameTime();
    else
    {
        isStarting = false;
    }
    if (MovingToGame)
        BlackTransparency += 0.65f * GetFrameTime();
    if (MovingToGame && BlackTransparency >= 1.0f) {
        map = target_map;
        StopSound(miku_sound);
    }

    DrawTexture(miku_img, -75, GetScreenHeight() - 20 + miku_offset, WHITE);
    if (GetMouseX() < 250 && GetMouseY() > GetScreenHeight() - 70) {
        miku_offset = lerp(miku_offset, -500, 10*GetFrameTime());
        if (!IsSoundPlaying(miku_sound)) {
            SetSoundVolume(miku_sound, 0.2f);
            ResumeSound(miku_sound);
        }
    } else {
        miku_offset = lerp(miku_offset, 0, 10*GetFrameTime());
        PauseSound(miku_sound);
    }

    LevelSelect();
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), ColorAlpha(BLACK, BlackTransparency));
}

std::string Menu::LeaveMenu() {
    return map;
}

void Menu::Quit() {
    UnloadTexture(title_img);
    UnloadSound(miku_sound);
    UnloadTexture(menu_img);
    UnloadTexture(button_img);
    UnloadTexture(miku_img);
}