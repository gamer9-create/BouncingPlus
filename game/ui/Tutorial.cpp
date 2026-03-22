//
// Created by lalit on 2/22/2026.
//

#include "Tutorial.h"

#include <iostream>

#include "UIManager.h"
#include "../Game.h"

// TODO: finish tutorial

void Tutorial::ActivateTutorial(UIManager* Parent)
{
    if (Stage <= 2)
    {
        for (std::shared_ptr e : Parent->game->GameEntities.Entities[EnemyType])
        {
            if (e->BoundingBox.y >= 5040)
            {
                shared_ptr<Enemy> enemy = dynamic_pointer_cast<Enemy>(e);
                if (enemy->Behavior != nullptr)
                    enemy->Behavior.reset();
            }
        }
    }

    if (Parent->game->MainPlayer->BoundingBox.y < 6768 && Stage < 0)
        Stage++;
    if (Parent->game->MainPlayer->BoundingBox.y < 6408 && Stage < 1)
        Stage++;
    if (Parent->game->MainPlayer->BoundingBox.y < 6120 && Stage < 2)
        Stage++;
    if (Parent->game->MainPlayer->BoundingBox.y < 5256 && Stage < 3)
        Stage++;
    //std::cout << stage << endl;

}
