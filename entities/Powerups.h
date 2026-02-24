//
// Created by lalit on 1/23/2026.
//

#ifndef BOUNCINGPLUS_POWERUPS_H
#define BOUNCINGPLUS_POWERUPS_H
#include <memory>
#include <string>

class Player;
class Game;

struct Powerup
{
    float Cooldown = 0;
    float Length = 0;
    std::string Name = "";
    std::string Texture = "";
    void complete(std::shared_ptr<Player> Owner);
    void undo(std::shared_ptr<Player> Owner);
};

struct SpeedPowerup : Powerup
{
    float Cooldown = 30;
    float Length = 3;
    std::string Name = "Speed";
    std::string Texture = "speed_powerup_icon";
    void complete(std::shared_ptr<Player> Owner);
    void undo(std::shared_ptr<Player> Owner);
};

class PowerupSystem
{
    public:
    std::shared_ptr<Player> Owner;
    Game* game;
    float CurrentCooldown;
    float CurrentLength;
    bool PowerupIsActive;
    Powerup *CurrentPowerup;
    PowerupSystem(std::shared_ptr<Player> Owner, Game &game);
    PowerupSystem();
    void Update();
    void Activate();
    void SetPowerup(Powerup *Powerup);
    virtual ~PowerupSystem();
};


#endif //BOUNCINGPLUS_POWERUPS_H