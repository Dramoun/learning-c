#ifndef BODY_H
#define BODY_H

#include "types.h"
#include "vec2.h"

typedef struct {
    Vec2 pos;
    Vec2 vel;
    Vec2 acc;
} Body;

Body bodyCreate(Vec2 pos, Vec2 vel);

void bodyApplyForce(Body *b, Vec2 force);
void bodyUpdate(Body *b, f32 dt);

#endif
