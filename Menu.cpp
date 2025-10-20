//
// Created by lalit on 10/16/2025.
//

#include "Menu.h"

#include <iostream>
#include <ostream>
#include <raymath.h>

using namespace std;

Menu::Menu()
{
    map = "";
    target_map = "";
    title_img = LoadTexture("assets/img/title.png");
    button_img = LoadTexture("assets/img/button.png");
    menu_img = LoadTexture("assets/img/menu_img.png");
    title_img_pos_y = -title_img.height;
    title_img_offset_y = 0;
    play_button_offset_y = -100;
    cam_x = 0;
    cam_x_targ = 0;
    BlackTransparency= 0.0f;
    menu_img_pos_y = 0;
    MovingToGame = false;
}

void Menu::Update() {
    cam_x = Lerp(cam_x, cam_x_targ, 2 * GetFrameTime());

    float off = -sin((GetTime()-20.0f) * 3.5f) * 15;
    float off2 = -sin((GetTime()+20.0f) * 3.5f) * 15;
    float off3 = -sin(GetTime() * 3.5f) * 15;

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
    DrawTexture(button_img, (int)(GetScreenWidth()/2.0f) - (int)(button_img.width/2.0f)-cam_x, play_button_offset_y + off3, WHITE);
    DrawText("PLAY", (int)(GetScreenWidth()/2.0f) - (int)(MeasureText("PLAY", 50)/2.0f)-cam_x, play_button_offset_y + 3 + off3, 50, WHITE);

    DrawText("LEVEL SELECT", (int)(GetScreenWidth()/2.0f) - (int)(MeasureText("LEVEL SELECT", 50)/2.0f)-cam_x-GetScreenWidth(), 250+off2, 50, WHITE);

    Rectangle play_bbox = {(GetScreenWidth()/2.0f) - (int)(button_img.width/2.0f)-cam_x, (float)play_button_offset_y +off3,150,56};
    if (CheckCollisionPointRec({GetMouseX()-cam_x, (float)GetMouseY()}, play_bbox)) {
        DrawRectangleLinesEx(play_bbox, 4, WHITE);
        if (IsMouseButtonPressed(0)) {
            cam_x_targ=-GetScreenWidth();
        }
    }
    if (MovingToGame)
        BlackTransparency += 0.5f * GetFrameTime();
    if (MovingToGame && BlackTransparency >= 1.0f)
        map = target_map;
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), ColorAlpha(BLACK, BlackTransparency));
}

std::string Menu::LeaveMenu() {
    return map;
}

void Menu::Quit() {
    UnloadTexture(title_img);
    UnloadTexture(button_img);
}