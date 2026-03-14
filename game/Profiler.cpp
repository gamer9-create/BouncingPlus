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

void Profiler::ProfilerLog(std::string Reason)
{
    StopLog();
    LastReason = Reason;
    LastTime = GetTime();
}

void Profiler::StopLog()
{
    if (!LastReason.empty())
    {
        Times[LastReason] = GetTime() - LastTime;
        LastReason = "";
        LastTime = -1;
    }
}

std::map<std::string, double> Profiler::Finish()
{
    StopLog();
    std::map<std::string, double> copy = Times;
    Times.clear();
    return copy;
}
