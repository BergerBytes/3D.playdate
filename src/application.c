//
// Created by Michael Berger on 7/14/23.
//

#include "application.h"
#include "renderer/renderer.h"

static void application_init(Application* app);

Application* application_create_default(PlaydateAPI* api) {
    Application* app = malloc(sizeof(Application));
    app->api = api;
    app->renderer = renderer_create(api, 30, 2);

    application_init(app);

    return app;
}

void application_destroy(Application* app) {
    renderer_cleanup(app->renderer);
    free(app);
}

static void application_init(Application* app) {}

int application_update(Application* app, PlaydateAPI* api) {
    renderer_draw(app->renderer, api);
    api->system->drawFPS(0, 0);
    return 1;
}
