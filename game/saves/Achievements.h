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
    std::string name;
    std::string description;
    std::string image;
};

class Achievements
{
public:
    std::unordered_map<std::string,Achievement> achievements;
    std::vector<std::string> earnedAchievements;
    Achievements();
    ~Achievements();
    void AwardAchievement(std::string name);
    nlohmann::json Save();
    void Load(nlohmann::json data);
};

#endif //BOUNCINGPLUS_ACHIEVEMENTS_H