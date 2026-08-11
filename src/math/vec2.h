#ifndef VEC2_H
#define VEC2_H

#include "../types.h"

typedef struct {
    f32 x;
    f32 y;
} Vec2;


/*
 * Adds two vectors together.
 *
 * Example:
 *   a = (2, 3)
 *   b = (4, 1)
 *
 *   result = (2 + 4, 3 + 1)
 *          = (6, 4)
 *
 * Useful for:
 *   - Moving a position
 *   - Combining forces
 *   - Adding velocity to position
 */
Vec2 vec2Add(Vec2 a, Vec2 b);


/*
 * Subtracts vector b from vector a.
 *
 * Example:
 *   a = (5, 4)
 *   b = (2, 1)
 *
 *   result = (5 - 2, 4 - 1)
 *          = (3, 3)
 *
 * Useful for:
 *   - Finding the direction from one position to another
 *   - Calculating movement
 *   - Collision calculations
 */
Vec2 vec2Sub(Vec2 a, Vec2 b);


/*
 * Multiplies both components of a vector by a scalar.
 *
 * Example:
 *   v = (2, 3)
 *   scalar = 2
 *
 *   result = (2 * 2, 3 * 2)
 *          = (4, 6)
 *
 * Useful for:
 *   - Changing movement speed
 *   - Applying acceleration
 *   - Scaling a direction vector
 */
Vec2 vec2Scale(Vec2 v, f32 scalar);


/*
 * Divides both components of a vector by a scalar.
 *
 * Example:
 *   v = (10, 6)
 *   divisor = 2
 *
 *   result = (10 / 2, 6 / 2)
 *          = (5, 3)
 *
 * Useful for:
 *   - Normalizing vectors
 *   - Reducing/scaling a vector
 *
 * Note:
 *   The divisor should not be 0.
 */
Vec2 vec2Divide(Vec2 v, f32 divisor);


/*
 * Returns the length (magnitude) of a vector.
 *
 * Uses the Pythagorean theorem:
 *
 *   length = sqrt(x² + y²)
 *
 * Example:
 *   v = (3, 4)
 *
 *   length = sqrt(3² + 4²)
 *          = sqrt(9 + 16)
 *          = sqrt(25)
 *          = 5
 *
 * Useful for:
 *   - Finding how fast something is moving
 *   - Finding the distance represented by a vector
 */
f32 vec2Length(Vec2 v);


/*
 * Returns the squared length of a vector.
 *
 * This calculates:
 *
 *   x² + y²
 *
 * instead of:
 *
 *   sqrt(x² + y²)
 *
 * Example:
 *   v = (3, 4)
 *
 *   lengthSquared = 3² + 4²
 *                 = 9 + 16
 *                 = 25
 *
 * Why use this?
 *   sqrtf() is more expensive than simple multiplication.
 *   When you only need to compare distances, the square root
 *   is unnecessary.
 *
 * Example:
 *   Instead of:
 *
 *       distance < 10
 *
 *   you can compare:
 *
 *       distanceSquared < 100
 *
 *   because 10² = 100.
 *
 * Especially useful for collision detection.
 */
f32 vec2LengthSquared(Vec2 v);


/*
 * Returns the distance between two positions.
 *
 * Internally:
 *
 *   1. Subtracts the two positions to get the difference vector.
 *   2. Calculates the length of that vector.
 *
 * Example:
 *
 *   a = (2, 3)
 *   b = (5, 7)
 *
 *   difference = (2, 3) - (5, 7)
 *              = (-3, -4)
 *
 *   distance = sqrt((-3)² + (-4)²)
 *            = 5
 *
 * Useful when you actually need the real distance.
 */
f32 vec2Distance(Vec2 a, Vec2 b);


/*
 * Returns the squared distance between two positions.
 *
 * Same idea as vec2Distance(), but without calculating sqrt().
 *
 * Example:
 *
 *   a = (2, 3)
 *   b = (5, 7)
 *
 *   difference = (-3, -4)
 *
 *   distanceSquared = (-3)² + (-4)²
 *                   = 25
 *
 * This is especially useful for collision detection because
 * you usually only need to know whether two objects are close
 * enough to collide.
 */
f32 vec2DistanceSquared(Vec2 a, Vec2 b);


/*
 * Converts a vector into a unit vector.
 *
 * A unit vector has a length of 1 while keeping the same
 * direction.
 *
 * Example:
 *
 *   v = (3, 4)
 *
 *   length = 5
 *
 *   normalized = (3 / 5, 4 / 5)
 *              = (0.6, 0.8)
 *
 * The result still points in the same direction, but its
 * length is now 1.
 *
 * Useful for:
 *   - Movement directions
 *   - Aiming
 *   - Enemy movement toward a player
 *   - Applying a speed to a direction
 *
 * Special case:
 *   A zero vector has no direction, so (0, 0) is returned.
 */
Vec2 vec2Normalize(Vec2 v);

#endif
