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
    CanLeaveMenu = false;
    title_img = LoadTexture("assets/img/title.png");
    button_img = LoadTexture("assets/img/button.png");
    title_img_pos_y = -title_img.height;
    title_img_offset_y = 0;
    play_button_offset_y = -100;
    BlackTransparency= 0.0f;
    MovingToGame = false;
}

void Menu::Update() {
    title_img_pos_y = Lerp(title_img_pos_y, -100, 5 * GetFrameTime());
    title_img_offset_y = sin(GetTime()*3.5f) * 15;
    if (abs(title_img_pos_y) - 100 <= 10)
        play_button_offset_y = Lerp(play_button_offset_y, 550, 5 * GetFrameTime());
    DrawTexture(title_img, (int)(GetScreenWidth()/2.0f) - (int)(title_img.width/2.0f), (int)title_img_pos_y - (int)title_img_offset_y, WHITE);
    DrawTexture(button_img, (int)(GetScreenWidth()/2.0f) - (int)(button_img.width/2.0f), play_button_offset_y - sin((GetTime()-20.0f) * 3.5f) * 15, WHITE);
    DrawText("PLAY", (int)(GetScreenWidth()/2.0f) - (int)(MeasureText("PLAY", 50)/2.0f), play_button_offset_y + 3 - sin((GetTime()-20.0f) * 3.5f) * 15, 50, WHITE);
    Rectangle play_bbox = {(GetScreenWidth()/2.0f) - (int)(button_img.width/2.0f), (float)play_button_offset_y - (float)sin((GetTime()-20.0f) * 3.5f) * 15,150,56};
    if (CheckCollisionPointRec(GetMousePosition(), play_bbox)) {
        DrawRectangleLinesEx(play_bbox, 4, WHITE);
        if (IsMouseButtonPressed(0)) {
            MovingToGame = true;
        }
    }
    if (MovingToGame)
        BlackTransparency += 0.5f * GetFrameTime();
    if (MovingToGame && BlackTransparency >= 1.0f)
        CanLeaveMenu = true;
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), ColorAlpha(BLACK, BlackTransparency));
}

bool Menu::LeaveMenu() {
    return CanLeaveMenu;
}

void Menu::Quit() {
    UnloadTexture(title_img);
    UnloadTexture(button_img);
}