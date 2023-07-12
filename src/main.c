#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"

static int update(void* userdata);
static void menuCallback(void *);
static void listFonts(const char* path, void* userdata);

const char* fontpath = "/System/Fonts/Roobert-10-Bold.pft";
LCDFont* font = NULL;

LCDBitmap* framebuffer = NULL;

#ifdef _WINDLL
__declspec(dllexport)
#endif
int eventHandler(PlaydateAPI* pd, PDSystemEvent event, uint32_t arg)
{
    (void)arg; // arg is currently only used for event = kEventKeyPressed

    if ( event == kEventInit )
    {
        const char* err;
        font = pd->graphics->loadFont(fontpath, &err);

        pd->file->listfiles("/System/Fonts", listFonts, pd, 0);

        if ( font == NULL )
            pd->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath, err);

        pd->display->setRefreshRate(24);
//        pd->display->setScale(2);

        // Note: If you set an update callback in the kEventInit handler, the system assumes the game is pure C and doesn't run any Lua code in the game
        pd->system->setUpdateCallback(update, pd);

        pd->system->addMenuItem("Dev Mode", menuCallback, NULL);

        framebuffer = pd->graphics->newBitmap(pd->display->getWidth(), pd->display->getHeight(), kColorBlack);
    }

    return 0;
}

#define TEXT_WIDTH 84
#define TEXT_HEIGHT 12

const char* text = "Hello from C!";

int x = 0;
int y = 0;
int dx = 1;
int dy = 1;

int lx = 0;
int ly = 0;

static int update(void* userdata)
{
    PlaydateAPI* pd = userdata;

    lx = x;
    ly = y;

    x += dx;
    y += dy;

    if ( x < 0 || x > pd->display->getWidth() - TEXT_WIDTH )
        dx = -dx;

    if ( y < 0 || y > pd->display->getHeight() - TEXT_HEIGHT )
        dy = -dy;


    pd->graphics->setFont(font);

//    int* points = malloc(sizeof(int) * 6);
//    points[0] = 0;
//    points[1] = 0;
//    points[2] = 100;
//    points[3] = 100;
//    points[4] = 0;
//    points[5] = 100;

//    pd->graphics->fillPolygon(3,points, kColorBlack, kPolygonFillNonZero);
//
//    // Clear text from last frame
////    pd->graphics->fillRect(lx, ly, TEXT_WIDTH, TEXT_HEIGHT, kColorWhite);
//
//    pd->graphics->setDrawMode(kDrawModeFillWhite);
//    pd->graphics->drawText(text, strlen(text), kASCIIEncoding, lx, ly);
//
//    // Draw text
//    pd->graphics->setDrawMode(kDrawModeFillBlack);
//    pd->graphics->drawText(text, strlen(text), kASCIIEncoding, x, y);

//    free(points);

    pd->graphics->clear(kColorWhite);

    uint8_t* data;
    int width, height, rowbytes;
    pd->graphics->getBitmapData(framebuffer, &width, &height, &rowbytes, NULL, &data);

    // Calculate the total amount of pixel data
    int total_pixels = width * height;

    int value = 0;
    // Iterate over the pixel data
    for (int pixelIndex = 0; pixelIndex < total_pixels; pixelIndex++) {
        int rowIndex = pixelIndex / width;  // get the row index
        int columnIndex = pixelIndex % width;  // get the column index
        int byteIndex = rowIndex * rowbytes + columnIndex / 8;  // calculate the byte index

        int brightness = (rowIndex + columnIndex) / 30;
        if (brightness == 0)
            brightness = 1;

        // Here's how you might manipulate the pixel data. In this case, we're just setting all pixels to be 'on'
        // But you could also read the value, modify it, etc.
        // NOTE: This assumes a 1bpp format where 1 is on and 0 is off.
        data[byteIndex] |= (value << (7 - (columnIndex % 8)));

        value = (rowIndex % brightness + columnIndex % brightness) == 0 ? 0 : 1;
    }

    pd->graphics->drawBitmap(framebuffer, 0, 0, kBitmapUnflipped);

    pd->system->drawFPS(0,0);

    return 1;
}

static void menuCallback(void *)
{
    printf("Menu item selected!\n");
}

static void listFonts(const char* path, void* userdata)
{
    PlaydateAPI* pd = userdata;
    printf("Font: %s\n", path);
    pd->system->logToConsole("Font: %s\n", path);
}