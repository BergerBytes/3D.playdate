//
// Created by Michael Berger on 7/26/23.
//

#include "triangle.h"
#include "vector3.h"

/**
 * Calculates the normal vector of a given triangle.
 *
 * @param triangle A pointer to the triangle structure containing the three points.
 * @return A Vector3 structure representing the normal vector of the triangle.
 */

Vector3 triangle_normal(Triangle* triangle) {
    Vector3 a = triangle->points[0];
    Vector3 b = triangle->points[1];
    Vector3 c = triangle->points[2];

    Vector3 ab = {
            .x = b.x - a.x,
            .y = b.y - a.y,
            .z = b.z - a.z
    };

    Vector3 ac = {
            .x = c.x - a.x,
            .y = c.y - a.y,
            .z = c.z - a.z
    };

    Vector3 crossProduct = vector3_cross_product(ab, ac);
    return vector3_normalize(crossProduct);
}
