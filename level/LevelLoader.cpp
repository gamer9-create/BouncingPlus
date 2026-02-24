//
// Created by Lalit on 10/20/2025.
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

std::map<std::string,json> LevelLoader::GetLevelsData()
{
    std::map<std::string,json> level_data = std::map<std::string,json>();
    std::vector<std::string> level_order = std::vector<std::string>();

    FilePathList list = LoadDirectoryFiles("assets\\maps");
    for (int i = 0; i < list.count; i++)
    {
        std::string c = string(list.paths[i]) + "\\metadata.json";

        if (!c.ends_with("LevelOrder.txt\\metadata.json"))
        {
            std::ifstream g(c);
            std::string e = string(list.paths[i]).substr(12, c.size()-14);
            json h = json::parse(g);
            level_data[e] = h;
        } else
        {
            std::ifstream g(string(list.paths[i]));
            std::string line;
            while(std::getline(g,line))
            {
                level_order.push_back(line);
            }
        }
    }

    for (int i = 0; i < level_order.size(); i++)
    {
        std::string s = level_order[i];
        level_data[s]["order"] = i;
    }

    return level_data;
}
