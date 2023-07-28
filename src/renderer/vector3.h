//
// Created by Michael Berger on 7/14/23.
//

#ifndef INC_3D_VECTOR3_H
#define INC_3D_VECTOR3_H

typedef struct {
    float x, y, z;
} Vector3;

// Operators:
Vector3 vector3_add(Vector3 a, Vector3 b);
Vector3 vector3_subtract(Vector3 a, Vector3 b);
Vector3 vector3_multiply(Vector3 a, Vector3 b);
Vector3 vector3_scalar_multiply(Vector3 vector, float scalar);

// Vector operations:
Vector3 vector3_cross_product(Vector3 a, Vector3 b);
float vector3_dot_product(Vector3 a, Vector3 b);
Vector3 vector3_normalize(Vector3 vector);
Vector3 vector3_squared_normalize(Vector3 vector);

// Utility functions:
Vector3 vector3_midpoint(Vector3 a, Vector3 b, Vector3 c);
float vector3_length(Vector3 vector);
float vector3_squared_length(Vector3 vector);

#endif //INC_3D_VECTOR3_H
