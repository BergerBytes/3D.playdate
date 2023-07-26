//
// Created by Michael Berger on 7/14/23.
//

#ifndef RENDERER_H
#define RENDERER_H

//#include "vector3.h"
//#include "Triangle.h"
//#include "mesh.h"
//#include "matrix4x4.h"

#include "pd_api.h"

typedef struct {
    int refreshRate;
    int scale;
    char* fontpath;

    int rows;
    int columns;
} Renderer;

Renderer* renderer_create(PlaydateAPI* api, int refreshRate, int scale);

void renderer_init(Renderer* renderer, PlaydateAPI* api);

void renderer_draw(Renderer* renderer, PlaydateAPI* api);

void renderer_cleanup(Renderer* renderer);


#endif /* RENDERER_H */