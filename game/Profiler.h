//
// Created by lalit on 2/22/2026.
//

#ifndef BOUNCINGPLUS_PROFILER_H
#define BOUNCINGPLUS_PROFILER_H
#include <map>
#include <string>

class Game;

class Profiler
{
public:
    Game *game;
    std::string last_reason = "";
    double last_time = -1;
    std::map<std::string,double> times;
    Profiler();
    Profiler(Game &game);
    ~Profiler();
    void log(std::string reason);
    void stop_log();
    std::map<std::string,double> finish();
};


#endif //BOUNCINGPLUS_PROFILER_H