//
// Created by lalit on 1/27/2026.
//

#include "Achievements.h"

#include <nlohmann/json.hpp>

Achievements::Achievements()
{
    achievements = std::unordered_map<std::string,Achievement>();
    earnedAchievements = std::vector<std::string>();
}

Achievements::~Achievements()
{
}

void Achievements::AwardAchievement(std::string name)
{
    earnedAchievements.push_back(name);
}

nlohmann::json Achievements::Save()
{
    nlohmann::json result;
    for (std::string s : earnedAchievements)
    {
        result.push_back(s);
    }
    return result;
}

void Achievements::Load(nlohmann::json data)
{
    earnedAchievements.clear();
    for (int i = 0; i < data.size(); i++)
    {
        std::string s = data[i].get<std::string>();
        earnedAchievements.push_back(s);
    }
}
