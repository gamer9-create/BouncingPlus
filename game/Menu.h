//
// Created by lalit on 10/16/2025.
//

#ifndef BOUNCINGPLUS_MENU_H
#define BOUNCINGPLUS_MENU_H
#include <raylib.h>
#include <string>

#include "../level/LevelLoader.h"

class Menu {
    std::map<std::string,json> level_data;
    std::string map;
    std::string target_map;
    Vector2 mouse_pos;
    float cam_x;
    float off, off2, off3;
    float cam_x_targ;
    float title_img_pos_y;
    float title_img_offset_y;
    float play_button_offset_y;
    float sett_button_offset_y;
    float cred_button_offset_y;
    bool fps_bar;
    Texture2D title_img;
    Texture2D menu_img;
    Texture2D button_img;
    Texture2D button_small_img;
    Texture2D button_small_img_red;
    Texture2D miku_img;
    Texture2D credits_img;
    Sound miku_sound;
    Sound slider_drag;
    Sound button_click;
    float miku_offset;
    float menu_img_pos_y;
    float BlackTransparency;
    bool MovingToGame;
    float* framerate;
    bool isStarting;
    float* master_volume;
    bool slider_bars;
    float last_played_prog;
    float last_played_prog_2;
    bool button(Rectangle rectangle, std::string text);
    Rectangle slider(Vector2 position, std::string text, float* value, float* last_played_progress, bool* prev_state, float min, float max);
    void LevelSelect();
    public:
        Menu(std::map<std::string,json> level_data, float* master_volume, float* framerate);
        void Update();
        void Reset();
        std::string LeaveMenu();
        void Quit();
};


#endif //BOUNCINGPLUS_MENU_H