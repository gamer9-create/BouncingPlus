//
// Created by lalit on 11/23/2025.
//

#ifndef BOUNCINGPLUS_FACEBOSS_H
#define BOUNCINGPLUS_FACEBOSS_H
#include "../Entity.h"
#include <raylib.h>

class FaceBoss : public Entity{
public:
    bool BossFightStarted; // bossfight tracker
    Vector2 StartPos; // boss starting position
    FaceBoss(Game &game, float bbox_x, float bbox_y);
    FaceBoss();
    ~FaceBoss()override;
    void Update()override;
    void Render();
};


#endif