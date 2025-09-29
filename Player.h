//
// Created by lalit on 8/27/2025.
//

#ifndef BOUNCINGPLUS_PLAYER_H
#define BOUNCINGPLUS_PLAYER_H
#include "Entity.h"
#include "Weapons.h"


class Player : public Entity {
    public:
        WeaponsSystem weaponsSystem;
        bool weaponsSystemInit = false;
        Player(float X, float Y, float Speed, Texture2D &PlayerTexture, Game &game);
        Player();
        virtual ~Player();
        void PhysicsUpdate(float dt);
        void Update();
};


#endif //BOUNCINGPLUS_PLAYER_H