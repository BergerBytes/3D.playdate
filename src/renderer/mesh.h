//
// Created by Michael Berger on 7/14/23.
//

#ifndef INC_3D_MESH_H
#define INC_3D_MESH_H

#include "triangle.h"

typedef struct {
    int triangleCount;
    Triangle* triangles;
} Mesh;

void destroy_mesh(Mesh* mesh) {
    free(mesh->triangles);
}

Mesh create_cube_mesh() {
    Mesh cube = {
            .triangleCount = 12,
            .triangles = malloc(sizeof(Triangle) * 12)
    };

    // South
    cube.triangles[0] = (Triangle) {
            .points = {
                    {.x = 0, .y = 0, .z = 0},
                    {.x = 0, .y = 1, .z = 0},
                    {.x = 1, .y = 1, .z = 0},
            }
    };
    cube.triangles[1] = (Triangle) {
            .points = {
                    {.x = 0, .y = 0, .z = 0},
                    {.x = 1, .y = 1, .z = 0},
                    {.x = 1, .y = 0, .z = 0},
            }
    };
    // East
    cube.triangles[2] = (Triangle) {
            .points = {
                    {.x = 1, .y = 0, .z = 0},
                    {.x = 1, .y = 1, .z = 0},
                    {.x = 1, .y = 1, .z = 1},
            }
    };
    cube.triangles[3] = (Triangle) {
            .points = {
                    {.x = 1, .y = 0, .z = 0},
                    {.x = 1, .y = 1, .z = 1},
                    {.x = 1, .y = 0, .z = 1},
            }
    };

    // North
    cube.triangles[4] = (Triangle) {
            .points = {
                    {.x = 1, .y = 0, .z = 1},
                    {.x = 1, .y = 1, .z = 1},
                    {.x = 0, .y = 1, .z = 1},
            }
    };
    cube.triangles[5] = (Triangle) {
            .points = {
                    {.x = 1, .y = 0, .z = 1},
                    {.x = 0, .y = 1, .z = 1},
                    {.x = 0, .y = 0, .z = 1},
            }
    };

    // West
    cube.triangles[6] = (Triangle) {
            .points = {
                    {.x = 0, .y = 0, .z = 1},
                    {.x = 0, .y = 1, .z = 1},
                    {.x = 0, .y = 1, .z = 0},
            }
    };
    cube.triangles[7] = (Triangle) {
            .points = {
                    {.x = 0, .y = 0, .z = 1},
                    {.x = 0, .y = 1, .z = 0},
                    {.x = 0, .y = 0, .z = 0},
            }
    };

    // Top
    cube.triangles[8] = (Triangle) {
            .points = {
                    {.x = 0, .y = 1, .z = 0},
                    {.x = 0, .y = 1, .z = 1},
                    {.x = 1, .y = 1, .z = 1},
            }
    };
    cube.triangles[9] = (Triangle) {
            .points = {
                    {.x = 0, .y = 1, .z = 0},
                    {.x = 1, .y = 1, .z = 1},
                    {.x = 1, .y = 1, .z = 0},
            }
    };

    // Bottom
    cube.triangles[10] = (Triangle) {
            .points = {
                    {.x = 1, .y = 0, .z = 1},
                    {.x = 0, .y = 0, .z = 1},
                    {.x = 0, .y = 0, .z = 0},
            }
    };
    cube.triangles[11] = (Triangle) {
            .points = {
                    {.x = 1, .y = 0, .z = 1},
                    {.x = 0, .y = 0, .z = 0},
                    {.x = 1, .y = 0, .z = 0},
            }
    };

    // Move origin to center
    for (int i = 0; i < cube.triangleCount; i++) {
        for (int p = 0; p < 3; p++) {
            cube.triangles[i].points[p].x -= 0.5f;
            cube.triangles[i].points[p].y -= 0.5f;
            cube.triangles[i].points[p].z -= 0.5f;
        }
    }

    return cube;
}

#endif //INC_3D_MESH_H
