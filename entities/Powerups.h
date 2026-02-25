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
    virtual void complete(std::shared_ptr<Player> Owner);
    virtual void undo(std::shared_ptr<Player> Owner);
};

struct SpeedPowerup : Powerup
{
    SpeedPowerup();
    void complete(std::shared_ptr<Player> Owner) override;
    void undo(std::shared_ptr<Player> Owner) override;
};

class PowerupSystem
{
    public:
    std::shared_ptr<Player> Owner;
    Game* game;
    float CurrentCooldown = 0;
    float CurrentLength = 0;
    bool PowerupIsActive = false;
    Powerup *CurrentPowerup = nullptr;
    PowerupSystem(std::shared_ptr<Player> Owner, Game &game);
    PowerupSystem();
    void Update();
    void Activate();
    void SetPowerup(Powerup *Powerup);
    virtual ~PowerupSystem();
};


#endif //BOUNCINGPLUS_POWERUPS_H