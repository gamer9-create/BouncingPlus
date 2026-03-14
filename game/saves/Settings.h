//
// Created by lalit on 3/13/2026.
//

#ifndef BOUNCINGPLUS_SETTINGS_H
#define BOUNCINGPLUS_SETTINGS_H

struct Settings
{
    std::map<std::string, nlohmann::json> LevelData;
    float FrameRate = -1;
    float Volume = 100;
    bool CursorWindowLock = false;
};

#endif //BOUNCINGPLUS_SETTINGS_H