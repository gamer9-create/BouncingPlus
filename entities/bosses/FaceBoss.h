//
// Created by lalit on 11/23/2025.
//

#ifndef BOUNCINGPLUS_FACEBOSS_H
#define BOUNCINGPLUS_FACEBOSS_H
#include "../Entity.h"

class FaceBoss : public Entity{
public:
    bool BossFightStarted;
    Vector2 StartPos;
    FaceBoss(Game &game, float bbox_x, float bbox_y);
    FaceBoss();
    virtual ~FaceBoss();
    void Update();
    void Render();
};


#endif //BOUNCINGPLUS_FACEBOSS_H