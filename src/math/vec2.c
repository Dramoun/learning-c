#include "vec2.h"
#include <math.h>


/*
 * Adds two vectors component-by-component.
 *
 * Example:
 *
 *   a = (2, 3)
 *   b = (4, 1)
 *
 *   result = (6, 4)
 *
 * This is useful when you want to move something:
 *
 *   position = position + velocity
 */
Vec2 vec2Add(Vec2 a, Vec2 b) {
    return (Vec2){
        a.x + b.x,
        a.y + b.y
    };
}


/*
 * Subtracts vector b from vector a component-by-component.
 *
 * Example:
 *
 *   a = (5, 4)
 *   b = (2, 1)
 *
 *   result = (3, 3)
 *
 * A very common use is finding the direction between
 * two positions:
 *
 *   direction = target - currentPosition
 */
Vec2 vec2Sub(Vec2 a, Vec2 b) {
    return (Vec2){
        a.x - b.x,
        a.y - b.y
    };
}


/*
 * Multiplies both components of a vector by a scalar.
 *
 * Example:
 *
 *   v = (2, 3)
 *   scalar = 2
 *
 *   result = (4, 6)
 *
 * Useful when applying speed:
 *
 *   velocity = direction * speed
 */
Vec2 vec2Scale(Vec2 v, f32 scalar) {
    return (Vec2){
        v.x * scalar,
        v.y * scalar
    };
}


/*
 * Divides both components of a vector by a scalar.
 *
 * Example:
 *
 *   v = (10, 6)
 *   divisor = 2
 *
 *   result = (5, 3)
 *
 * This is mainly useful for normalization, where we divide
 * a vector by its own length.
 *
 * Be careful not to divide by zero.
 */
Vec2 vec2Divide(Vec2 v, f32 divisor) {
    return (Vec2){
        v.x / divisor,
        v.y / divisor
    };
}


/*
 * Calculates the length (magnitude) of a vector.
 *
 * Formula:
 *
 *   sqrt(x² + y²)
 *
 * Example:
 *
 *   v = (3, 4)
 *
 *   sqrt(3² + 4²)
 *   = sqrt(9 + 16)
 *   = sqrt(25)
 *   = 5
 */
f32 vec2Length(Vec2 v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}


/*
 * Calculates the squared length of a vector.
 *
 * Formula:
 *
 *   x² + y²
 *
 * Unlike vec2Length(), this does NOT use sqrtf().
 *
 * Example:
 *
 *   v = (3, 4)
 *
 *   result = 3² + 4²
 *          = 9 + 16
 *          = 25
 *
 * Why is this useful?
 *
 * If you only want to compare distances, you don't need
 * the actual distance.
 *
 * Instead of:
 *
 *   distance < 5
 *
 * you can use:
 *
 *   distanceSquared < 25
 *
 * This avoids the square root calculation.
 */
f32 vec2LengthSquared(Vec2 v) {
    return v.x * v.x + v.y * v.y;
}


/*
 * Calculates the distance between two points.
 *
 * First we subtract the points to get the vector between them,
 * then calculate the length of that vector.
 *
 * Example:
 *
 *   a = (2, 3)
 *   b = (5, 7)
 *
 *   difference = a - b
 *              = (-3, -4)
 *
 *   distance = sqrt(9 + 16)
 *            = 5
 */
f32 vec2Distance(Vec2 a, Vec2 b) {
    return vec2Length(vec2Sub(a, b));
}


/*
 * Calculates the squared distance between two points.
 *
 * This is the same concept as vec2Distance(), but avoids sqrtf().
 *
 * Example:
 *
 *   a = (2, 3)
 *   b = (5, 7)
 *
 *   difference = (-3, -4)
 *
 *   distanceSquared = 9 + 16
 *                   = 25
 *
 * This is useful for collision detection.
 */
f32 vec2DistanceSquared(Vec2 a, Vec2 b) {
    return vec2LengthSquared(vec2Sub(a, b));
}


/*
 * Converts a vector into a unit vector.
 *
 * A unit vector has a length of exactly 1.
 *
 * Example:
 *
 *   v = (3, 4)
 *
 *   length = 5
 *
 *   normalized:
 *
 *       (3 / 5, 4 / 5)
 *       = (0.6, 0.8)
 *
 * The direction stays the same, but the length becomes 1.
 *
 * This lets us separate:
 *
 *   direction
 *
 * from:
 *
 *   speed
 *
 * For example:
 *
 *   direction = vec2Normalize(target - position);
 *   velocity = vec2Scale(direction, speed);
 *
 * If the vector is (0, 0), it has no direction, so we return
 * (0, 0) rather than dividing by zero.
 */
Vec2 vec2Normalize(Vec2 v) {
    f32 length = vec2Length(v);

    if (length == 0.0f) {
        return (Vec2){
            0.0f,
            0.0f
        };
    }

    return vec2Divide(v, length);
}
