//
// Created by 7906200 on 10/20/2025.
//

#include "LevelLoader.h"
#include <fstream>
#include <iostream>
#include <raylib.h>
#include "vector"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

using namespace std;

LevelLoader::LevelLoader()
{
}

std::unordered_map<std::string,json> LevelLoader::GetLevelsData()
{
    std::unordered_map<std::string,json> level_data = std::unordered_map<std::string,json>();

    FilePathList list = LoadDirectoryFiles("assets\\maps");
    for (int i = 0; i < list.count; i++)
    {
        std::string c = string(list.paths[i]) + "\\metadata.json";
        std::ifstream g(c);
        std::string e = string(list.paths[i]).substr(12, c.size()-14);
        level_data[e] = json::parse(g);
    }

    return level_data;
}
