//
// Created by lalit on 9/28/2025.
//

#ifndef BOUNCINGPLUS_UI_H
#define BOUNCINGPLUS_UI_H
#include <raylib.h>
#include <string>
class Game;

Color GetHealthColor(float Percent, float Armor = 0.0f);

class UI {
    Game *game;
    RenderTexture2D WeaponUITexture;
    RenderTexture2D DeathScreen;
    RenderTexture2D PauseScreen;
    int WeaponSlotIndex = -1;
    float WeaponSlotOffset = 0;
    float WeaponSlotSize = 0;
    float HealthBarSize = 250;
    float UITransparency = 1.0f;
    float DeathTextAnimRot = 0.0f;
    public:
    UI(Game &game);
    UI();
    bool button(Rectangle rectangle, std::string text) {
    void PauseMenu();
    void GameUI();
    void Quit();
};


#endif //BOUNCINGPLUS_UI_H