//
// Created by Michael Berger on 7/14/23.
//

#ifndef INC_3D_APPLICATION_H
#define INC_3D_APPLICATION_H

#include <pd_api.h>
#include "renderer/renderer.h"

typedef struct {
    PlaydateAPI* api;
    Renderer* renderer;
} Application;

Application* application_create_default(PlaydateAPI* api);

void application_destroy(Application* app);

int application_update(Application* app, PlaydateAPI* api);

#endif //INC_3D_APPLICATION_H
