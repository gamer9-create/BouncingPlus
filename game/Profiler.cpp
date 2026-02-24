//
// Created by lalit on 2/22/2026.
//

#include "Profiler.h"

#include "Game.h"

Profiler::Profiler()
{
}

Profiler::Profiler(Game& game)
{
    this->game = &game;
}

Profiler::~Profiler()
{
}

void Profiler::log(std::string reason)
{
    stop_log();
    last_reason = reason;
    last_time = GetTime();
}

void Profiler::stop_log()
{
    if (!last_reason.empty())
    {
        times[last_reason] = GetTime() - last_time;
        last_reason = "";
        last_time = -1;
    }
}

std::map<std::string, double> Profiler::finish()
{
    stop_log();
    std::map<std::string, double> copy = times;
    times.clear();
    return copy;
}
