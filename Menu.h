//
// Created by lalit on 10/16/2025.
//

#ifndef BOUNCINGPLUS_MENU_H
#define BOUNCINGPLUS_MENU_H
#include <raylib.h>


class Menu {
    bool CanLeaveMenu;
    float title_img_pos_y;
    float title_img_offset_y;
    float play_button_offset_y;
    Texture2D title_img;
    Texture2D button_img;
    float BlackTransparency;
    bool MovingToGame;
    public:
        Menu();
        void Update();
        bool LeaveMenu();
        void Quit();
};


#endif //BOUNCINGPLUS_MENU_H