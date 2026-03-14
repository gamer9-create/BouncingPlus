//
// Created by lalit on 3/14/2026.
//

#include "Settings.h"

void Settings::DisplaySettings(Vector2 Position, float Offset1, float Offset2)
{
    Slider({Position.x + (float)GetScreenWidth() * 1.5f,Position.y + 300 + (Offset1 + Offset2)/4}, GetMousePosition(), this->UIAssets.SliderDrag, this->UIAssets.ButtonSmallImg, "VOLUME", &this->Volume, &LastVolumeBar, &VolumeBarInteraction, 0, 100.0f);
    Slider({Position.x + (float)GetScreenWidth() * 1.5f,Position.y + 300 + (Offset1 + Offset2)/4 + 60}, GetMousePosition(), this->UIAssets.SliderDrag, this->UIAssets.ButtonSmallImg, "FRAMERATE", &this->FrameRate, &LastFrameRateBar, &FrameRateBarInteraction, 30, 240);
}
