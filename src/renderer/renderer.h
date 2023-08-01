//
// Created by Michael Berger on 7/14/23.
//

#ifndef RENDERER_H
#define RENDERER_H

#include "pd_api.h"
#include "matrix4x4.h"

typedef struct {
    int refreshRate;
    int scale;
    char* fontpath;

    int rows;
    int columns;

    Vector3 directionalLight;
    Vector3 cameraPosition;
    Matrix4x4 projectionMatrix;
} Renderer;

Renderer* renderer_create(PlaydateAPI* api, int refreshRate, int scale);

void renderer_init(Renderer* renderer, PlaydateAPI* api);

void renderer_draw(Renderer* renderer, PlaydateAPI* api);

void renderer_cleanup(Renderer* renderer);

#endif /* RENDERER_H */