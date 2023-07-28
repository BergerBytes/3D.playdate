//
// Created by Michael Berger on 7/26/23.
//

#include <math.h>
#include "vector3.h"

/**
 * @brief Adds two Vector3 objects and returns the result.
 *
 * This function takes two Vector3 objects, adds their corresponding
 * components together, and returns a new Vector3 object containing
 * the sum of the components.
 *
 * @param a Pointer to the first Vector3 object.
 * @param b Pointer to the second Vector3 object.
 * @return The result of adding the two Vector3 objects.
 */

inline Vector3 vector3_add(Vector3 a, Vector3 b) {
    Vector3 result = {
            .x = a.x + b.x,
            .y = a.y + b.y,
            .z = a.z + b.z
    };
    return result;
}

/**
 * @brief Subtracts two 3D vectors.
 *
 * This function takes two 3D vectors as input and subtracts them to produce a new vector.
 *
 * @param a The first vector to subtract.
 * @param b The second vector to subtract.
 * @return The resulting vector after subtraction.
 */

Vector3 vector3_subtract(Vector3 a, Vector3 b) {
    Vector3 result = {
            .x = a.x - b.x,
            .y = a.y - b.y,
            .z = a.z - b.z
    };
    return result;
}

/**
 * Multiplies two Vector3 objects element-wise and returns the result.
 *
 * @param a - Pointer to the first Vector3 object.
 * @param b - Pointer to the second Vector3 object.
 * @return The multiplication result as a new Vector3 object.
 */

inline Vector3 vector3_multiply(Vector3 a, Vector3 b) {
    Vector3 result = {
            .x = a.x * b.x,
            .y = a.y * b.y,
            .z = a.z * b.z
    };
    return result;
}

/**
 * Multiplies a Vector3 by a scalar value.
 *
 * @param vector Pointer to a Vector3 struct representing the input vector.
 * @param scalar The scalar value to multiply the vector by.
 * @return The result of multiplying the vector by the scalar.
 *
 * @par Example:
 * @code
 * Vector3 vec = {1.0f, 2.0f, 3.0f};
 * Vector3 result = vector3_scalar_multiply(&vec, 2.0f);
 * // result.x will be 2.0f
 * // result.y will be 4.0f
 * // result.z will be 6.0f
 * @endcode
 */

inline Vector3 vector3_scalar_multiply(Vector3 vector, float scalar) {
    Vector3 result = {
            .x = vector.x * scalar,
            .y = vector.y * scalar,
            .z = vector.z * scalar
    };
    return result;
}

/**
 * Calculates the cross product of two 3D vectors.
 *
 * @param a Pointer to the first vector.
 * @param b Pointer to the second vector.
 * @return The cross product of the two vectors.
 */

inline Vector3 vector3_cross_product(Vector3 a, Vector3 b) {
    Vector3 result = {
            .x = a.y * b.z - a.z * b.y,
            .y = a.z * b.x - a.x * b.z,
            .z = a.x * b.y - a.y * b.x
    };
    return result;
}

/**
 * @brief Calculates the dot product of two 3D vectors.
 *
 * The dot product of two vectors is a scalar value that represents the cosine of the angle between them,
 * scaled by the product of their magnitudes. For two vectors u and v, the dot product is defined as:
 *
 * dot(u, v) = u.x * v.x + u.y * v.y + u.z * v.z
 *
 * @param a The first 3D vector.
 * @param b The second 3D vector.
 * @return The dot product of the two vectors.
 */

float vector3_dot_product(Vector3 a, Vector3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

/**
 * @brief Normalizes a Vector3.
 *
 * This function calculates the length of the given Vector3 and divides each component of the vector
 * by the length to normalize it.
 *
 * @param vector The Vector3 to be normalized.
 *
 * @return The normalized Vector3.
 *
 * @note The function assumes that the input vector is not a NULL pointer.
 * @warning The function does not handle zero-length vectors, which may result in a division by zero error.
 */

inline Vector3 vector3_normalize(Vector3 vector) {
    float length = vector3_length(vector);
    Vector3 result = {
            .x = vector.x / length,
            .y = vector.y / length,
            .z = vector.z / length
    };
    return result;
}

/**
 * @brief Squared normalize a Vector3
 *
 * This function calculates the squared normalized version of a Vector3.
 * It scales the vector components by the inverse of the vector length squared.
 *
 * @param vector A pointer to the Vector3 to be squared normalized
 * @return The squared normalized Vector3
 */

inline Vector3 vector3_squared_normalize(Vector3 vector) {
    float length = vector3_squared_length(vector);
    Vector3 result = {
            .x = vector.x / length,
            .y = vector.y / length,
            .z = vector.z / length
    };
    return result;
}

/**

* Calculates the midpoint between three 3D vectors.
*
* @param a A pointer to the first Vector3.
* @param b A pointer to the second Vector3.
* @param c A pointer to the third Vector3.
* @return The Vector3 midpoint.
*
* @note The input vectors a, b, and c should not be modified.
*
* @warning The caller is responsible for allocating memory for the result Vector3 and deallocating it when no longer needed.
*
* @see Vector3
*
*/

inline Vector3 vector3_midpoint(Vector3 a, Vector3 b, Vector3 c) {
    Vector3 result = {
            .x = (a.x + b.x + c.x) / 3.0f,
            .y = (a.y + b.y + c.y) / 3.0f,
            .z = (a.z + b.z + c.z) / 3.0f
    };
    return result;
}

/**
 * @brief Calculates the length of a Vector3.
 *
 * This function calculates the length of a Vector3 using the formula:
 * length = sqrt(vector->x * vector->x + vector->y * vector->y + vector->z * vector->z)
 *
 * @param vector A pointer to the Vector3 structure.
 *
 * @return The length of the Vector3.
 */

inline float vector3_length(Vector3 vector) {
    return sqrtf(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

/**
 * @brief Calculates the squared length of a Vector3.
 *
 * This function calculates the squared length of the provided Vector3 object by
 * summing the squares of the x, y, and z components.
 *
 * @param vector Pointer to a Vector3 object.
 * @return The squared length of the Vector3.
 */

inline float vector3_squared_length(Vector3 vector) {
    return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
}

