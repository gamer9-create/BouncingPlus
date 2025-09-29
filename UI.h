//
// Created by lalit on 9/28/2025.
//

#ifndef BOUNCINGPLUS_UI_H
#define BOUNCINGPLUS_UI_H
#include <raylib.h>
class Game;

class UI {
    public:
    Game *game;
    RenderTexture2D WeaponUITexture;
    UI(Game &game);
    UI();
    void WeaponUI();
};


#endif //BOUNCINGPLUS_UI_H