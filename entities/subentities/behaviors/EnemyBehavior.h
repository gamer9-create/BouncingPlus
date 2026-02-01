//
// Created by lalit on 1/31/2026.
//

#ifndef BOUNCINGPLUS_ENEMYBEHAVIOR_H
#define BOUNCINGPLUS_ENEMYBEHAVIOR_H

class Enemy;
class Game;

class EnemyBehavior
{
    public:
    Enemy* Owner;
    Game* game;
    EnemyBehavior();
    EnemyBehavior(Enemy& Owner, Game& game);
    virtual void Update();
};


#endif //BOUNCINGPLUS_ENEMYBEHAVIOR_H