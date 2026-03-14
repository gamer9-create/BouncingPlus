//
// Created by lalit on 1/27/2026.
//

#include "Saves.h"
#include "Achievements.h"
#include <fstream>

// TODO: finish saving system

Saves::Saves()
{
    MainAchievements = Achievements();
    CurrentSave["achievements"] = MainAchievements.EarnedAchievements;
    CurrentSave["beaten_levels"] = {};
}

Saves::~Saves()
{
}

void Saves::Save(std::string Filepath)
{
}

void Saves::Load(std::string Filepath)
{
    std::ifstream g(Filepath);
    CurrentSave = nlohmann::json::parse(g);
    MainAchievements.Load(CurrentSave["achievements"]);
}
