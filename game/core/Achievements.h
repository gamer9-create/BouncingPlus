//
// Created by lalit on 1/27/2026.
//

#ifndef BOUNCINGPLUS_ACHIEVEMENTS_H
#define BOUNCINGPLUS_ACHIEVEMENTS_H
#include <string>
#include <unordered_map>
#include <nlohmann/json_fwd.hpp>

struct Achievement
{
    std::string Name;
    std::string Description;
    std::string Image;
};

class Achievements
{
public:
    std::unordered_map<std::string,Achievement> AchievementsMap;
    std::vector<std::string> EarnedAchievements;
    Achievements();
    ~Achievements();
    void AwardAchievement(std::string Name);
    nlohmann::json Save();
    void Load(nlohmann::json Data);
};

#endif //BOUNCINGPLUS_ACHIEVEMENTS_H