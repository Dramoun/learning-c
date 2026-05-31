#include "body.h"
#include "vec2.h"
#include "types.h"

Body bodyCreate(Vec2 pos, Vec2 vel)
{
    Body b;
    b.pos = pos;
    b.vel = vel;
    b.acc = (Vec2){0, 0};
    return b;
}

void bodyApplyForce(Body *b, Vec2 force)
{
    // simple model: acceleration += force (mass = 1)
    b->acc = vec2Add(b->acc, force);
}

void bodyUpdate(Body *b, f32 dt)
{
    // integrate velocity
    b->vel = vec2Add(b->vel, vec2Scale(b->acc, dt));

    // integrate position
    b->pos = vec2Add(b->pos, vec2Scale(b->vel, dt));

    // reset acceleration each frame
    b->acc = (Vec2){0, 0};
}
