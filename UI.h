//
// Created by lalit on 9/28/2025.
//

#ifndef BOUNCINGPLUS_UI_H
#define BOUNCINGPLUS_UI_H
#include <raylib.h>
class Game;

Color GetHealthColor(float Percent, float Armor = 0.0f);

class UI {
    public:
    Game *game;
    RenderTexture2D WeaponUITexture;
    RenderTexture2D DeathScreen;
    Texture2D HealthBarTexture;
    int WeaponSlotIndex = -1;
    float WeaponSlotOffset = 0;
    float WeaponSlotSize = 0;
    float HealthBarSize = 250;
    float UITransparency = 1.0f;
    float DeathTextAnimRot = 0.0f;
    UI(Game &game);
    UI();
    void WeaponUI();
    void Quit();
};


#endif //BOUNCINGPLUS_UI_H