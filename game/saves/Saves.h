//
// Created by lalit on 1/27/2026.
//

#ifndef BOUNCINGPLUS_SAVES_H
#define BOUNCINGPLUS_SAVES_H
#include <nlohmann/json.hpp>

#include "Achievements.h"


class Saves
{
    public:
    nlohmann::json CurrentSave;
    Achievements MainAchievements;
    Saves();
    ~Saves();
    void Save(std::string Filepath);
    void Load(std::string Filepath);
};


#endif //BOUNCINGPLUS_SAVES_H