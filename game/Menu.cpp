//
// Created by lalit on 10/16/2025.
//

#include "Menu.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <raymath.h>
using json = nlohmann::json;

using namespace std;

Rectangle Menu::slider(Vector2 position, std::string text, float* value, float* last_played_progress, bool* prev_state, float min, float max)
{
    float percentage = std::max(std::min((*value - min) / (max - min), 1.0f), 0.0f);

    float slider_size = 350;

    float fnt_size = 35;
    float tx_size = MeasureText(text.c_str(), fnt_size);
    float w = tx_size + slider_size + 48;
    float h = fnt_size + 16;
    position.x -= w/2;
    Rectangle rec = {position.x, position.y,w, h};

    Rectangle slider_rec = {position.x + tx_size + 32, position.y + (rec.height / 2) - (fnt_size / 2.5f), slider_size, fnt_size / 1.25f};

    Rectangle green_slider_rec = {slider_rec.x, slider_rec.y, slider_rec.width * percentage, slider_rec.height};

    Rectangle red_slider_rec = {slider_rec.x + (slider_rec.width * percentage), slider_rec.y, slider_rec.width * (1.0f - percentage), slider_rec.height};

    bool colliding = CheckCollisionPointRec(mouse_pos, slider_rec);
    bool clicking = IsMouseButtonDown(0);
    if (!clicking)
        *prev_state = false;
    bool dragging = (*prev_state || colliding) && clicking;

    if (dragging)
    {
        *prev_state = true;
        float distance = std::min(std::max(mouse_pos.x - slider_rec.x, 0.0f), slider_size);
        float percent = distance / slider_size;
        *value = min + (max - min) * percent;
        if (abs(*value - *last_played_progress) >= 0.1f && !IsSoundPlaying(slider_drag))
        {
            PlaySound(slider_drag);
            *last_played_progress = *value;
        }
        if (*value >= 0.95f)
            *value = 1.0f;
        if (*value <= 0.05f)
            *value = 0.0f;
        if (*value >= 0.45f && *value <= 0.55f)
            *value = 0.5f;
    }

    DrawRectangleRec({rec.x - cam_x,rec.y,rec.width,rec.height}, ColorAlpha(BLACK, 0.5f));
    DrawRectangleRec({green_slider_rec.x - cam_x,green_slider_rec.y,green_slider_rec.width,green_slider_rec.height}, GREEN);
    DrawRectangleRec({red_slider_rec.x - cam_x,red_slider_rec.y,red_slider_rec.width,red_slider_rec.height}, RED);
    DrawTextPro(GetFontDefault(), text.c_str(), {position.x + 16 - cam_x, position.y + (rec.height / 2.0f) - (fnt_size / 2.0f)}, {0, 0}, 0, fnt_size, 1, WHITE);


    float siz = fnt_size + 5;
    if (colliding)
        siz = fnt_size + 7;
    DrawTexturePro(button_small_img, {0, 0, 56, 56}, {green_slider_rec.x - cam_x + green_slider_rec.width, green_slider_rec.y + green_slider_rec.height/2, siz, siz}, {siz / 2, siz / 2}, 0, WHITE);

    return rec;
}

Menu::Menu(std::map<std::string,json> level_data, float* master_volume)
{
    this->level_data = level_data;
    this->master_volume = master_volume;
    title_img = LoadTexture("assets/ui/title.png");
    button_small_img = LoadTexture("assets/ui/button_small.png");
    button_img = LoadTexture("assets/ui/button.png");
    menu_img = LoadTexture("assets/ui/menu_img.png");
    miku_img = LoadTexture("assets/ui/miku.png");
    miku_sound = LoadSound("assets/ui/lovely_cavity.mp3");
    credits_img = LoadTexture("assets/ui/credits.png");
    button_click = LoadSound("assets/ui/button_click.wav");
    slider_drag = LoadSound("assets/ui/slider_dragging.wav");
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
    sett_button_offset_y = -200;
    cred_button_offset_y = -300;
    slider_bars = false;
    cam_x = 0;
    cam_x_targ = 0;
    last_played_prog = 1.0f;
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

    int f= 50;
    if (CheckCollisionPointRec(mouse_pos, rectangle))
    {
        rectangle.width += 20;
        rectangle.height += 20;
        rectangle.x -= 10;
        rectangle.y -=10;
        f += 10;
    }

    float tx_size = MeasureText(text.c_str(), f);
    float mul = f / (tx_size / (rectangle.width-10));
    tx_size = MeasureText(text.c_str(), mul);
    DrawTexturePro(button_img, {0,0,(float)button_img.width,(float)button_img.height}, {rectangle.x-cam_x, rectangle.y, rectangle.width, rectangle.height}, {0, 0}, 0, WHITE);
    DrawText(text.c_str(), rectangle.x-cam_x + rectangle.width/2 - tx_size/2, rectangle.y + rectangle.height/2 - (mul/2), mul, WHITE);

    if (CheckCollisionPointRec(mouse_pos, rectangle)) {
        DrawRectangleLinesEx({rectangle.x-cam_x,rectangle.y,rectangle.width,rectangle.height}, 4, WHITE);
        if (IsMouseButtonPressed(0) && !IsSoundPlaying(button_click))
        {
            PlaySound(button_click);
        }
        return IsMouseButtonPressed(0);
    }
    return false;
}

void Menu::LevelSelect()
{
    Rectangle play_bbox = {(GetScreenWidth()/2.0f) - GetScreenWidth() - (int)(button_img.width/2.0f), GetScreenHeight() - 106 + off,150,56};
    if (button(play_bbox, "BACK")) {
        cam_x_targ=0;
    }
    DrawText("LEVEL SELECT", (int)(GetScreenWidth()/2.0f) - (int)(MeasureText("LEVEL SELECT", 50)/2.0f)-cam_x-GetScreenWidth(), 100+(off2/1.5f), 50, WHITE);
    int i = 0;
    for (auto& [name, data] : level_data)
    {
        i = data["order"];
        Rectangle r = {-710.0f, (float)165 + (140 * i)-off3, (float)MeasureText(name.c_str(), 90)+20.0f, 110.0f};
        DrawRectangleRec({r.x-cam_x,r.y,r.width,r.height}, ColorAlpha(BLACK, 0.5f));
        Color c = RED;
        if (target_map==name) {
            c = GREEN;
            float comb = (off + off2 + off3)/3;
            DrawRectangle(-(GetScreenWidth()-125)-cam_x, 200-comb, GetScreenWidth()-900, GetScreenHeight()-450, ColorAlpha(BLACK, 0.5f));
            DrawText(name.c_str(), -(GetScreenWidth()-145)-cam_x, 220-comb, 50, WHITE);

            std::string description = data["description"];

            if (!description.empty())
            {
                float alloc_size = GetScreenWidth() - 940;
                int lines = ceil(MeasureText(description.c_str(), 25) / alloc_size);

                int str_prog = 0;
                int last_str_prog = 0;

                for (int i = 0; i < lines; i++)
                {
                    last_str_prog = str_prog;
                    while (MeasureText(description.substr(last_str_prog, str_prog - last_str_prog).c_str(), 25) < alloc_size && str_prog < description.length())
                        str_prog++;

                    std::string txt = description.substr(last_str_prog, str_prog - last_str_prog);
                    if (txt.front() == ' ')
                        txt.erase(txt.begin());
                    //description.substr(max((float)(i * (description.size() / lines)), 0.0f), min((i+1) * (description.size() / lines), description.size())).c_str()

                    DrawText(txt.c_str(), -(GetScreenWidth()-145)-cam_x, 270-comb + (35 * i), 25, WHITE);
                }
            }

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
    if (abs(title_img_pos_y + 100) <= 10)
        play_button_offset_y = Lerp(play_button_offset_y, 550, 5 * GetFrameTime());
    if (abs(play_button_offset_y - 550) <= 45)
        sett_button_offset_y = Lerp(sett_button_offset_y, 624, 7 * GetFrameTime());
    if (abs(sett_button_offset_y - 624) <= 45)
        cred_button_offset_y = Lerp(cred_button_offset_y, 698, 9 * GetFrameTime());

    menu_img_pos_y += GetFrameTime()*15;
    float zoom = 1.35f;
    DrawTexturePro(menu_img, {0, menu_img_pos_y, (float)GetScreenWidth(), (float)GetScreenHeight()},
        {GetScreenWidth()/2 - (GetScreenWidth() * zoom)/2 - cam_x/10, GetScreenHeight()/2 - (GetScreenHeight() * zoom)/2, (float)GetScreenWidth() * zoom, (float)GetScreenHeight() * zoom},
        {0,0},
        0,
        WHITE
        );

    DrawTexture(title_img, (int)(GetScreenWidth()/2.0f) - (int)(title_img.width/2.0f)-cam_x, (int)title_img_pos_y - (int)title_img_offset_y, WHITE);

    slider({(float)GetScreenWidth() * 1.5f,300 + (off + off2)/4}, "VOLUME", master_volume, &last_played_prog, &slider_bars, 0, 1.0f);

    Rectangle play_bbox = {(GetScreenWidth()/2.0f) - (int)(button_img.width/2.0f), (float)play_button_offset_y +off3,150,56};
    if (button(play_bbox, "PLAY")) {
        cam_x_targ=-GetScreenWidth();
    }
    DrawTexture(credits_img,(GetScreenWidth()*2)-cam_x, 0, WHITE);

    if (button({GetScreenWidth()*2 + (GetScreenWidth() / 2.0f) - 75.0f, GetScreenHeight() - 106.0f, 150, 56}, "BACK") ||
        button({GetScreenWidth() + (GetScreenWidth() / 2.0f) - 75.0f, GetScreenHeight() - 106.0f + (off2 + off3)/2, 150, 56}, "BACK"))
        cam_x_targ=0;

    if (button({(GetScreenWidth()/2.0f) - (int)(button_img.width/2.0f), (float)sett_button_offset_y + (off2 + off3)/2,150,56}, "SETTINGS"))
        cam_x_targ=GetScreenWidth();

    if (button({(GetScreenWidth()/2.0f) - (int)(button_img.width/2.0f), (float)cred_button_offset_y +off2,150,56}, "CREDITS"))
        cam_x_targ=GetScreenWidth()*2;

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
    UnloadTexture(button_small_img);
    UnloadTexture(menu_img);
    UnloadTexture(button_img);
    UnloadTexture(miku_img);
    UnloadTexture(credits_img);
    UnloadSound(slider_drag);
    UnloadSound(button_click);
}