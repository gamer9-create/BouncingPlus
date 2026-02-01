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
    Achievements achievements;
    Saves();
    ~Saves();
    void Save(std::string filepath);
    void Load(std::string filepath);
};


#endif //BOUNCINGPLUS_SAVES_H