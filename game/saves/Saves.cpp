//
// Created by lalit on 1/27/2026.
//

#include "Saves.h"
#include "Achievements.h"
#include <fstream>

Saves::Saves()
{
    achievements = Achievements();
    CurrentSave["achievements"] = achievements.earnedAchievements;
    CurrentSave["beaten_levels"] = {};
}

Saves::~Saves()
{
}

void Saves::Save(std::string filepath)
{
}

void Saves::Load(std::string filepath)
{
    std::ifstream g(filepath);
    CurrentSave = nlohmann::json::parse(g);
    achievements.Load(CurrentSave["achievements"]);
}
