#include "vec2.h"

#include <math.h>

Vec2 vec2Add(Vec2 a, Vec2 b) {
    return (Vec2){
        a.x + b.x,
        a.y + b.y
    };
}

Vec2 vec2Sub(Vec2 a, Vec2 b) {
    return (Vec2){
        a.x - b.x,
        a.y - b.y
    };
}

Vec2 vec2Scale(Vec2 v, f32 scalar) {
    return (Vec2){
        v.x * scalar,
        v.y * scalar
    };
}

Vec2 vec2Devide(Vec2 v, f32 devider) {
    return (Vec2){
        v.x / devider,
        v.y / devider
    };
}

f32 vec2Length(Vec2 v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

Vec2 vec2Normalize(Vec2 v) {
    f32 length = vec2Length(v);

    if (length == 0.0f) {
        return (Vec2){ 0.0f, 0.0f };
    }

    return (Vec2){
        v.x / length,
        v.y / length
    };
}
