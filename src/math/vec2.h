#ifndef VEC2_H
#define VEC2_H

#include "types.h"

typedef struct {
    f32 x;
    f32 y;
} Vec2;

Vec2 vec2Add(Vec2 a, Vec2 b);
Vec2 vec2Sub(Vec2 a, Vec2 b);

Vec2 vec2Scale(Vec2 v, f32 scalar);

f32 vec2Length(Vec2 v);

Vec2 vec2Normalize(Vec2 v);

#endif
