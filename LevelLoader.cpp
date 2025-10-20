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
    cout << "LevelLoader::GetLevelsData()" << endl;
    std::unordered_map<std::string,json> level_data = std::unordered_map<std::string,json>();

    cout << "List" << endl;
    FilePathList list = LoadDirectoryFiles("assets\\maps");
    for (int i = 0; i < list.count; i++)
    {
        cout << list.paths[i]<<endl;
        std::string c = string(list.paths[i]) + "\\metadata.json";
        std::cout << c<< std::endl;
        std::ifstream g(c);
        std::string e = string(list.paths[i]);
        //e.
        //level_data[e] = json::parse(c);
    }

    return level_data;
}
