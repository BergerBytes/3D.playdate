//
// Created by Michael Berger on 7/26/23.
//

#include <math.h>
#include "matrix4x4.h"
#include <stddef.h>

/**
 * @brief Generates and returns a 4x4 identity matrix.
 *
 * The function creates a 4x4 identity matrix and returns it.
 * An identity matrix is a square matrix with ones on the diagonal
 * and zeros elsewhere. This matrix represents a transformation that
 * leaves an object unchanged when applied to it.
 *
 * @return The 4x4 identity matrix.
 */

Matrix4x4 matrix4x4_identity(void) {
    Matrix4x4 result = {
            .m = {
                    {1.0f, 0.0f, 0.0f, 0.0f},
                    {0.0f, 1.0f, 0.0f, 0.0f},
                    {0.0f, 0.0f, 1.0f, 0.0f},
                    {0.0f, 0.0f, 0.0f, 1.0f}
            }
    };
    return result;
}

/**
 * Calculates a 4x4 projection matrix.
 *
 * @param fovDegree The field of view in degrees.
 * @param aspectRatio The aspect ratio of the viewport.
 * @param near The distance to the near clipping plane.
 * @param far The distance to the far clipping plane.
 * @return The calculated projection matrix.
 */

Matrix4x4 matrix4X4_projection(int fovDegree, float aspectRatio, float near, float far) {
    float fov = (float) fovDegree * PI / 180.0f;

    Matrix4x4 result = {0};

    if (aspectRatio == 0)
        return result;
    if (fov <= 0 || fov >= PI)
        return result;
    if (far - near <= 0)
        return result;

    float tanHalfFOV = tanf(fov / 2.0f);

    float zRange = far - near;

    result.m[0][0] = 1.0f / (tanHalfFOV * aspectRatio);
    result.m[1][1] = 1.0f / tanHalfFOV;
    result.m[2][2] = (-near - far) / zRange;
    result.m[3][2] = (-far * near) / (far - near);
    result.m[2][3] = 1.0f;
    result.m[3][3] = 0.0f;

    return result;
}

/**
 * @brief Multiply a Vector3 by a 4x4 matrix.
 *
 * This function multiplies a Vector3 by a 4x4 matrix using homogeneous coordinates. The result is stored in the "out" vector.
 *
 * @param in The input Vector3.
 * @param out The output Vector3.
 * @param matrix The 4x4 matrix.
 */

void vector3_multiply_matrix4x4(const Vector3* in, Vector3* out, const Matrix4x4* matrix) {
    out->x = in->x * matrix->m[0][0] + in->y * matrix->m[1][0] + in->z * matrix->m[2][0] + matrix->m[3][0];
    out->y = in->x * matrix->m[0][1] + in->y * matrix->m[1][1] + in->z * matrix->m[2][1] + matrix->m[3][1];
    out->z = in->x * matrix->m[0][2] + in->y * matrix->m[1][2] + in->z * matrix->m[2][2] + matrix->m[3][2];
    float w = in->x * matrix->m[0][3] + in->y * matrix->m[1][3] + in->z * matrix->m[2][3] + matrix->m[3][3];

    float threshold = 0.0001f;
    if (fabsf(w) > threshold) {
        out->x /= w;
        out->y /= w;
        out->z /= w;
    }
}

/**
 * @brief Multiplies two 4x4 matrices and stores the result in another 4x4 matrix.
 *
 * This function multiplies two 4x4 matrices 'a' and 'b' and stores the result in 'result'.
 *
 * @param a    Pointer to the first input 4x4 matrix.
 * @param b    Pointer to the second input 4x4 matrix.
 * @param result    Pointer to the output 4x4 matrix where the result will be stored.
 *
 * @note The matrices 'a', 'b', and 'result' must not be NULL.
 */

void matrix4x4_multiply(const Matrix4x4* a, const Matrix4x4* b, Matrix4x4* result) {
    if (a == NULL || b == NULL || result == NULL) {
        return;
    }

    Matrix4x4 temp = {0};

    for (int row = 0; row < 4; row++)
        for (int col = 0; col < 4; ++col)
            for (int i = 0; i < 4; ++i)
                temp.m[row][col] += a->m[row][i] * b->m[i][col];

    *result = temp;
}

void matrix4x4_rotate_y(Matrix4x4* matrix, float angle, Matrix4x4* result) {
    float sinAngle = sinf(angle);
    float cosAngle = cosf(angle);

    Matrix4x4 rotationMatrix = {
            .m = {
                    {cosAngle,  0.0f, sinAngle, 0.0f},
                    {0.0f,      1.0f, 0.0f,     0.0f},
                    {-sinAngle, 0.0f, cosAngle, 0.0f},
                    {0.0f,      0.0f, 0.0f,     1.0f}
            }
    };

    matrix4x4_multiply(matrix, &rotationMatrix, result);
}
