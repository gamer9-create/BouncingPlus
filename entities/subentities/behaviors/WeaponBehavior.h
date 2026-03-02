//
// Created by lalit on 1/31/2026.
//

#ifndef BOUNCINGPLUS_WEAPONBEHAVIOR_H
#define BOUNCINGPLUS_WEAPONBEHAVIOR_H
#include <raylib.h>

#include "EnemyBehavior.h"


class WeaponBehavior : public EnemyBehavior
{
    public:
    double CoverRefreshTimer;
    Vector2 CoverPosition;
    bool FoundCover;
    bool CoverSearching;
    WeaponBehavior();
    WeaponBehavior(Enemy& Owner, Game& game);
    ~WeaponBehavior();
    void MoveForCover();
    void Update() override;
};


#endif //BOUNCINGPLUS_WEAPONBEHAVIOR_H