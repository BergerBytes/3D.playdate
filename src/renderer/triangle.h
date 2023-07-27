//
// Created by Michael Berger on 7/14/23.
//

#ifndef INC_3D_TRIANGLE_H
#define INC_3D_TRIANGLE_H

#include "vector3.h"

typedef struct {
    Vector3 points[3];
} Triangle;

Vector3 triangle_normal(Triangle* triangle);

#endif //INC_3D_TRIANGLE_H
