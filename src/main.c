#include <stdio.h>

#include "pd_api.h"
#include "bayer.h"

static int update(void *userdata);

const char *fontpath = "/System/Fonts/Roobert-10-Bold.pft";
LCDFont *font = NULL;

int TOTAL_PIXELS = 0;
const int SCALE = 2;
const int SCALED_LCD_ROWS = LCD_ROWS / SCALE;
const int SCALED_LCD_COLUMNS = LCD_COLUMNS / SCALE;

#ifdef _WINDLL
__declspec(dllexport)
#endif

int eventHandler(PlaydateAPI *pd, PDSystemEvent event, uint32_t arg) {
    (void) arg; // arg is currently only used for event = kEventKeyPressed

    if (event == kEventInit) {
        const char *err;
        font = pd->graphics->loadFont(fontpath, &err);

        if (font == NULL)
            pd->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath, err);

        pd->display->setRefreshRate(50);
        pd->display->setScale(SCALE);
        TOTAL_PIXELS = (SCALED_LCD_COLUMNS * SCALED_LCD_ROWS);

        // Note: If you set an update callback in the kEventInit handler, the system assumes the game is pure C and doesn't run any Lua code in the game
        pd->system->setUpdateCallback(update, pd);
    }

    return 0;
}

int reverse = 0;
const int BAYER_TABLE = BAYER_8x8;
const int maxGradient = 30;
const int minGradient = 10;
int gradient = 0;

static int update(void *userdata) {
    PlaydateAPI *pd = userdata;

    pd->graphics->clear(kColorClear);
    uint8_t *data = pd->graphics->getFrame();

    // Calculate the total amount of pixel data

//    float deltaTime = pd->system->getElapsedTime() * 20;

    gradient += 1 * (reverse ? -1 : 1);
    if (gradient > maxGradient) {
        gradient = maxGradient;
        reverse = 1;
    } else if (gradient < minGradient) {
        gradient = minGradient;
        reverse = 0;
    }

    // Iterate over the pixel data
    for (int pixelIndex = 0; pixelIndex < TOTAL_PIXELS; pixelIndex++) {
        int rowIndex = pixelIndex / SCALED_LCD_COLUMNS;  // calculate row
        int columnIndex = pixelIndex % SCALED_LCD_COLUMNS;  // calculate column
        int byteIndex = rowIndex * LCD_ROWSIZE + columnIndex / 8;  // calculate byte index in data array

        // Get the corresponding value from the Bayer matrix
        int bayer = bayerValue(rowIndex, columnIndex, BAYER_TABLE);

        // TODO: Implement a real way to get the brightness of a pixel.
        int pixelBrightness = (rowIndex + columnIndex) / ((SCALED_LCD_ROWS + SCALED_LCD_COLUMNS) / 64);

        if (pixelBrightness > bayer) {
            // Set the pixel 'on'
            data[byteIndex] |= (1 << (7 - (columnIndex % 8)));
        } else {
            // Set the pixel 'off'
            data[byteIndex] &= ~(1 << (7 - (columnIndex % 8)));
        }
    }

    pd->system->drawFPS(0, 0);

//    pd->system->resetElapsedTime();

    return 1;
}