//
// Created by lalit on 10/16/2025.
//

#ifndef BOUNCINGPLUS_MENU_H
#define BOUNCINGPLUS_MENU_H
#include <raylib.h>
#include <string>

#include "../../level/LevelLoader.h"
#include "../saves/Settings.h"

class Menu {
    std::string Map;
    std::string TargetMap;
    Vector2 MousePos;
    float CameraX;
    float Offset1, Offset2, Offset3;
    float CameraTargetX;
    float TitleImgY;
    float TitleImgOffsetY;
    float PlayButtonOffsetY;
    float SettingsButtonOffsetY;
    float CreditsButtonOffsetY;
    float MikuOffset;
    float MenuImgOffsetY;
    float BlackTransparency;
    bool MovingToGame;
    bool isStarting;
    Settings* GameSettings;
    void LevelSelect();
    public:
        Menu(Settings& GameSettings);
        void Update();
        void Reset();
        std::string LeaveMenu();
        void Quit();
};


#endif //BOUNCINGPLUS_MENU_H