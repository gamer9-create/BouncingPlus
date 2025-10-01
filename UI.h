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
    int WeaponSlotIndex = -1;
    float WeaponSlotOffset = 0;
    float WeaponSlotSize = 0;
    UI(Game &game);
    UI();
    void WeaponUI();
};


#endif //BOUNCINGPLUS_UI_H