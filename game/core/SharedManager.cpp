//
// Created by lalit on 3/14/2026.
//

#include "SharedManager.h"

#include <iostream>

void SharedManager::DisplaySettings(Vector2 Position, float Offset1, float Offset2)
{
    if (!ControlBindingsMenu)
    {
        Panel(Rectangle{Position.x + 200, Position.y + 175, GetScreenWidth() - 400.0f, GetScreenHeight() - 400.0f}, "SETTINGS", -(Offset1 + Offset2)/4);
        Slider({Position.x + (float)GetScreenWidth() /2,Position.y + 300 + (Offset1 + Offset2)/4}, GetMousePosition(), this->UIAssets.SliderDrag, this->UIAssets.ButtonSmallImg, "VOLUME", &this->Volume, &LastVolumeBar, &VolumeBarInteraction, 0, 100.0f);
        Slider({Position.x + (float)GetScreenWidth() /2,Position.y + 300 + (Offset1 + Offset2)/4 + 60}, GetMousePosition(), this->UIAssets.SliderDrag, this->UIAssets.ButtonSmallImg, "FRAMERATE", &this->FrameRate, &LastFrameRateBar, &FrameRateBarInteraction, 30, 240);
        Checkmark({Position.x + (float)GetScreenWidth() /2,Position.y + 300 + (Offset1 + Offset2)/4 + 120}, GetMousePosition(), this->UIAssets.SliderDrag, this->UIAssets.ButtonSmallImg, this->UIAssets.ButtonSmallRedImg, "CURSOR WINDOW LOCK", &CursorWindowLock);
        Checkmark({Position.x + (float)GetScreenWidth() /2,Position.y + 300 + (Offset1 + Offset2)/4 + 180}, GetMousePosition(), this->UIAssets.SliderDrag, this->UIAssets.ButtonSmallImg, this->UIAssets.ButtonSmallRedImg, "SHAKE CAMERA", &ShakeCamera);
        Checkmark({Position.x + (float)GetScreenWidth() /2,Position.y + 300 + (Offset1 + Offset2)/4 + 240}, GetMousePosition(), this->UIAssets.SliderDrag, this->UIAssets.ButtonSmallImg, this->UIAssets.ButtonSmallRedImg, "FULLSCREEN", &Fullscreen);
    } else
        ControlBindings(Position,Offset1,Offset2);
}

void SharedManager::ControlBindings(Vector2 Position, float Offset1, float Offset2)
{

}

void SharedManager::Update()
{
    if (FrameRate != LastFrameRate)
        SetTargetFPS((int)FrameRate);
    LastFrameRate = FrameRate;

    if (Controls.IsControlPressed("fullscreen"))
        Fullscreen = !Fullscreen;

    if (IsWindowFullscreen() != Fullscreen)
        ToggleFullscreen();

    if (CursorWindowLock && !IsCursorOnScreen())
        SetMousePosition(min(max(GetMouseX(), 25), GetScreenWidth() - 25), min(max(GetMouseY(), 25), GetScreenHeight() - 25));

    SetMasterVolume(Volume / 100.0f);

    Controls.Update();
}

void SharedManager::Quit()
{
    UIAssets.Quit();
    Controls.Quit();
}
