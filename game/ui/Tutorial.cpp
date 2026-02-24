//
// Created by lalit on 2/22/2026.
//

#include "Tutorial.h"

#include <iostream>

#include "UIManager.h"
#include "../Game.h"

// TODO: finish tutorial

void Tutorial::do_stuff(UIManager* parent)
{
    if (stage <= 2)
    {
        for (std::shared_ptr e : parent->game->MainEntityManager.Entities[EnemyType])
        {
            if (e->BoundingBox.y >= 5040)
            {
                shared_ptr<Enemy> enemy = dynamic_pointer_cast<Enemy>(e);
                if (enemy->Behavior != nullptr)
                    enemy->Behavior.reset();
            }
        }
    }

    if (parent->game->MainPlayer->BoundingBox.y < 6768 && stage < 0)
        stage++;
    if (parent->game->MainPlayer->BoundingBox.y < 6408 && stage < 1)
        stage++;
    if (parent->game->MainPlayer->BoundingBox.y < 6120 && stage < 2)
        stage++;
    if (parent->game->MainPlayer->BoundingBox.y < 5256 && stage < 3)
        stage++;
    //std::cout << stage << endl;

}
