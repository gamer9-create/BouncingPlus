//
// Created by lalit on 11/5/2025.
//

#include "SoundManager.h"
#include "../Game.h"
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
    for (auto& [name,value] : Sounds) {
        StopSound(value);
    }
}

void SoundManager::Update() {
    int i = 0;
    if (game->DebugDraw)
        DrawText("Sound Cache (>10 is bad)", 350+game->GameCamera.RaylibCamera.target.x,50+game->GameCamera.RaylibCamera.target.y, 10, RED);
    for (auto& [name,value] : CachedAliases) {
        if (game->DebugDraw)
            DrawText((to_string(i) + " " + to_string(value.size())).c_str(), 350+game->GameCamera.RaylibCamera.target.x,50+((i+1)*10)+game->GameCamera.RaylibCamera.target.y, 10, RED);
        if (game->GameSpeed != 1.0f)
        {
            for (Sound& sound : value)
            {
                if (IsSoundPlaying(sound))
                {

                }
            }
        }
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

void SoundManager::PlaySoundM(std::string SoundName, float SoundVolume, float SoundPitch) {
    if (Sounds.contains(SoundName) && IsSoundValid(Sounds[SoundName])) {
        for (Sound& CachedSound : CachedAliases[SoundName]) {
            if (IsSoundValid(CachedSound) && !IsSoundPlaying(CachedSound)) {
                SetSoundVolume(CachedSound, SoundVolume);
                SetSoundPitch(CachedSound, SoundPitch);
                PlaySound(CachedSound);
                return;
            }
        }

        Sound CachedSound = LoadSoundAlias(Sounds[SoundName]);
        SetSoundVolume(CachedSound, SoundVolume);
        SetSoundPitch(CachedSound, SoundPitch);
        PlaySound(CachedSound);
        CachedAliases[SoundName].push_back(CachedSound);
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
