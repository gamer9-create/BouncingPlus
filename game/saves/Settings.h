//
// Created by lalit on 3/13/2026.
//

#ifndef BOUNCINGPLUS_SETTINGS_H
#define BOUNCINGPLUS_SETTINGS_H
#include <map>
#include <string>
#include <nlohmann/json_fwd.hpp>

#include "../ui/UIUtils.h"

using namespace std;
using namespace nlohmann;

struct Settings
{
    map<string, json> LevelData;
    UIAssets UIAssets;
    float FrameRate = -1;
    float Volume = 100;
    bool CursorWindowLock = false;
    bool VolumeBarInteraction = false;
    bool FrameRateBarInteraction = false;
    float LastVolumeBar = 100.0f;
    float LastFrameRateBar = 60.0f;
    void DisplaySettings(Vector2 Position, float Offset1, float Offset2);
};

#endif //BOUNCINGPLUS_SETTINGS_H