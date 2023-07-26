#include <stdio.h>

#include "pd_api.h"
#include "application.h"

static int updateApplication(PlaydateAPI* api);

Application* application;

#ifdef _WINDLL
__declspec(dllexport)
#endif

/**
 * @brief Handles system events and performs necessary actions.
 *
 * This function is used to handle system events and perform the required actions based on the event type.
 *
 * @param*/
 
int eventHandler(PlaydateAPI* pd, PDSystemEvent event, uint32_t arg) {
    if (event == kEventInit) {
        application = application_create_default(pd);
        pd->system->setUpdateCallback((int (*)(void*)) updateApplication, pd);
    }

    if (event == kEventTerminate)
        application_destroy(application);

    return 0;
}


/**
 * \brief Update the application using the PlaydateAPI.
 *
 * This function updates the application using the provided PlaydateAPI object.
 *
 * \param api Pointer to the PlaydateAPI object.
 *
 * \return The result of the application update. See the documentation of the
 *         PlaydateAPI for more information on possible return values.
 */

static int updateApplication(PlaydateAPI* api) {
    return application_update(application, api);
}