//
// Created by Lalit on 10/20/2025.
//

#ifndef LEVELLOADER_H
#define LEVELLOADER_H
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class LevelLoader {
public:
    LevelLoader();
    std::unordered_map<std::string,json> GetLevelsData();
};



#endif //LEVELLOADER_H
