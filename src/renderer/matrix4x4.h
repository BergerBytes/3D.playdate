//
// Created by Michael Berger on 7/26/23.
//

#ifndef INC_3D_MATRIX4X4_H
#define INC_3D_MATRIX4X4_H

#include "vector3.h"

#define PI 3.14159265f

typedef struct {
    _Alignas(16) float m[4][4];
} Matrix4x4;

Matrix4x4 matrix4x4_identity(void);

Matrix4x4 matrix4X4_projection(int fovDegree, float aspectRatio, float near, float far);

void vector3_multiply_matrix4x4(const Vector3* in, Vector3* out, const Matrix4x4* matrix);

void matrix4x4_multiply(const Matrix4x4* a, const Matrix4x4* b, Matrix4x4* result);

void matrix4x4_rotate_y(Matrix4x4* matrix, float angle, Matrix4x4* result);

#endif //INC_3D_MATRIX4X4_H
