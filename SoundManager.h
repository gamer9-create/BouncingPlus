//
// Created by lalit on 11/5/2025.
//

#ifndef BOUNCINGPLUS_SOUNDMANAGER_H
#define BOUNCINGPLUS_SOUNDMANAGER_H
#include <raylib.h>
#include <unordered_map>
#include <string>

class Game;

class SoundManager {
    public:
    Game *game;
    std::unordered_map<std::string, Sound> Sounds;
    std::unordered_map<std::string, std::vector<Sound>> CachedAliases;
    SoundManager(Game &game);
    SoundManager();
    ~SoundManager();
    void PlaySoundM(std::string sound, float volume = 1.0f, float pitch = 1.0f);
    void Reset();
    void Update();
    void Quit();
};


#endif //BOUNCINGPLUS_SOUNDMANAGER_H