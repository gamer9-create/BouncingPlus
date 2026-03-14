//
// Created by lalit on 1/27/2026.
//

#include "Achievements.h"

#include <nlohmann/json.hpp>

// TODO: finish achievement system

Achievements::Achievements()
{
    AchievementsMap = std::unordered_map<std::string,Achievement>();
    EarnedAchievements = std::vector<std::string>();
}

Achievements::~Achievements()
{
}

void Achievements::AwardAchievement(std::string Name)
{
    EarnedAchievements.push_back(Name);
}

nlohmann::json Achievements::Save()
{
    nlohmann::json result;
    for (std::string s : EarnedAchievements)
    {
        result.push_back(s);
    }
    return result;
}

void Achievements::Load(nlohmann::json Data)
{
    EarnedAchievements.clear();
    for (int i = 0; i < Data.size(); i++)
    {
        std::string s = Data[i].get<std::string>();
        EarnedAchievements.push_back(s);
    }
}
