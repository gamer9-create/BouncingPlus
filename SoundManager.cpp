//
// Created by lalit on 11/5/2025.
//

#include "SoundManager.h"
#include "Game.h"
#include <filesystem>
#include <iostream>

SoundManager::SoundManager(Game &game) {
    this->game = &game;
    std::string path = "assets\\sounds";
    for (const auto & entry : std::filesystem::directory_iterator(path)) {
        std::string p = entry.path().filename().string();
        p.erase(p.end() - 4, p.end());
        Sound sound = LoadSound(entry.path().string().c_str());
        Sounds.insert({p, sound});
        std::vector<Sound> s = std::vector<Sound>();
        CachedAliases[p] = s;
    }
}

SoundManager::SoundManager() {
}

SoundManager::~SoundManager() {
}

void SoundManager::Clear() {
    for (auto& [name,value] : CachedAliases) {
        for (Sound& sound : value) {
            StopSound(sound);
        }
    }
}

void SoundManager::Update() {
    int i = 0;
    if (game->DebugDraw)
        DrawText("sound management stuff. if the numbers on the right are higher than like 11 somethings wrong", 350,350, 25, RED);
    for (auto& [name,value] : CachedAliases) {
        if (game->DebugDraw)
            DrawText((to_string(i) + " " + to_string(value.size())).c_str(), 350,350+((i+1)*25), 25, RED);
        std::erase_if(value, [&](Sound& sound) {
            if (value.size() > 10 && ((IsSoundValid(sound) && !IsSoundPlaying(sound)) || !IsSoundValid(sound))) {
                if (IsSoundValid(sound))
                    UnloadSoundAlias(sound);
                return true;
            }
            return false;
        });
        i += 1;
    }
}

void SoundManager::PlaySoundM(std::string sound, float volume, float pitch) {
    if (Sounds.contains(sound)) {
        for (Sound& CachedSound : CachedAliases[sound]) {
            if (IsSoundValid(CachedSound) && !IsSoundPlaying(CachedSound)) {
                SetSoundVolume(CachedSound, volume);
                SetSoundPitch(CachedSound, pitch);
                PlaySound(CachedSound);
                return;
            }
        }

        Sound CachedSound = LoadSoundAlias(Sounds[sound]);
        SetSoundVolume(CachedSound, volume);
        SetSoundPitch(CachedSound, pitch);
        PlaySound(CachedSound);
        CachedAliases[sound].push_back(CachedSound);
    }
}

void SoundManager::Quit() {
    for (auto& [name,value] : CachedAliases) {
        for (Sound& sound : value) {
            if (IsSoundValid(sound)) {
                UnloadSoundAlias(sound);
            }
        }
    }
    for (auto& [name,value] : Sounds) {
        if (IsSoundValid(value))
            UnloadSound(value);
    }
}
