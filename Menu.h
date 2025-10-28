//
// Created by lalit on 10/16/2025.
//

#ifndef BOUNCINGPLUS_MENU_H
#define BOUNCINGPLUS_MENU_H
#include <raylib.h>
#include <string>

#include "LevelLoader.h"

class Menu {
    std::unordered_map<std::string,json> level_data;
    std::string map;
    std::string target_map;
    Vector2 mouse_pos;
    float cam_x;
    float off, off2, off3;
    float cam_x_targ;
    float title_img_pos_y;
    float title_img_offset_y;
    float play_button_offset_y;
    Texture2D title_img;
    Texture2D menu_img;
    Texture2D button_img;
    Texture2D miku_img;
    Sound miku_sound;
    float miku_offset;
    float menu_img_pos_y;
    float BlackTransparency;
    bool MovingToGame;
    bool button(Rectangle rectangle, std::string text);
    void LevelSelect();
    public:
        Menu(std::unordered_map<std::string,json> level_data);
        void Update();
        std::string LeaveMenu();
        void Quit();
};


#endif //BOUNCINGPLUS_MENU_H