//
// Created by lalit on 10/19/2025.
//

#ifndef BOUNCINGPLUS_PARTICLESYSTEM_H
#define BOUNCINGPLUS_PARTICLESYSTEM_H

class Game;

class ParticleManager {
    Game *game;
    public:
    ParticleManager();
    ParticleManager(Game &game);
    void Update();
    void Quit();
};

#endif //BOUNCINGPLUS_PARTICLESYSTEM_H