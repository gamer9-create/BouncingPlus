//
// Created by lalit on 10/16/2025.
//

#ifndef BOUNCINGPLUS_MENU_H
#define BOUNCINGPLUS_MENU_H
#include <raylib.h>
#include <string>


class Menu {
    std::string map;
    std::string target_map;
    float cam_x;
    float cam_x_targ;
    float title_img_pos_y;
    float title_img_offset_y;
    float play_button_offset_y;
    Texture2D title_img;
    Texture2D menu_img;
    Texture2D button_img;
    float menu_img_pos_y;
    float BlackTransparency;
    bool MovingToGame;
    public:
        Menu();
        void Update();
        std::string LeaveMenu();
        void Quit();
};


#endif //BOUNCINGPLUS_MENU_H