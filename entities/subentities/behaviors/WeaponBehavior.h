//
// Created by lalit on 1/31/2026.
//

#ifndef BOUNCINGPLUS_WEAPONBEHAVIOR_H
#define BOUNCINGPLUS_WEAPONBEHAVIOR_H
#include "EnemyBehavior.h"


class WeaponBehavior : public EnemyBehavior
{
    public:
    WeaponBehavior();
    WeaponBehavior(Enemy& Owner, Game& game);
    void Update();
};


#endif //BOUNCINGPLUS_WEAPONBEHAVIOR_H