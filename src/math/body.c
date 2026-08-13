#include "body.h"
#include "vec2.h"
#include "types.h"

Body bodyCreate(Vec2 pos, Vec2 vel)
{
  Body b;
  b.pos = pos;
  b.vel = vel;
  b.acc = (Vec2){0, 0};
  b.hitbox = 0.5f;
  return b;
}

void bodyApplyForce(Body *b, Vec2 force)
{
  b->acc = vec2Add(b->acc, force);
}

void bodyUpdate(Body *b, f32 dt)
{
  bodyUpdateWithAcceleration(b, dt, (Vec2) {0,0});
}

void bodyUpdateWithAcceleration(Body *b, f32 dt, Vec2 newAcc)
{
  // integrate velocity
  b->vel = vec2Add(b->vel, vec2Scale(b->acc, dt));
  // integrate position
  b->pos = vec2Add(b->pos, vec2Scale(b->vel, dt));
  b->acc = newAcc;
}

bool twoBodyCollisionCheck(Body *a, Body *b) {
  f32 radiusSum = a->hitbox + b->hitbox;
  f32 twoBodyDistanceSquared = vec2DistanceSquared(a->pos, b->pos);
  
  return twoBodyDistanceSquared <= radiusSum * radiusSum;
} 
