//
// Created by Michael Berger on 7/14/23.
//

#include "renderer.h"
#include "bayer.h"
#include "mesh.h"

LCDFont* font = NULL;

const int BAYER_TABLE = BAYER_8;

void set_pixel_on(uint8_t* data, int byteIndex, int columnIndex);

void set_pixel_off(uint8_t* data, int byteIndex, int columnIndex);

int calculate_byte_index(int rowIndex, int columnIndex);

void renderer_draw_line(uint8_t* frame, int x1, int y1, int x2, int y2);

/**
 * \brief Creates a new renderer.
 *
 * This function creates a new renderer object with the specified refresh rate and scale.
 *
 * \param api The PlaydateAPI object used for rendering.
 * \param refreshRate The refresh rate of the renderer.
 * \param scale The scale factor applied to the renderer.
 *
 * \return A pointer to the newly created Renderer object.
 */

Renderer* renderer_create(PlaydateAPI* api, int refreshRate, int scale) {
    Renderer* renderer = malloc(sizeof(Renderer));
    renderer->refreshRate = refreshRate;
    renderer->scale = scale;
    renderer->rows = LCD_ROWS / scale;
    renderer->columns = LCD_COLUMNS / scale;

    renderer->fontpath = "/System/Fonts/Roobert-10-Bold.pft";
    renderer_init(renderer, api);

    return renderer;
}

/**
 * \brief Initializes the renderer.
 *
 * This function initializes the renderer with the given renderer and Playdate API objects.
 * It sets the refresh rate and scale of the display based on the renderer settings.
 * It also loads the specified font from the provided font path.
 * If the font loading fails, an error message is printed to the system log.
 *
 * \param renderer Pointer to the Renderer object to be initialized.
 * \param api Pointer to the PlaydateAPI object.
 * \return None.
 */

void renderer_init(Renderer* renderer, PlaydateAPI* api) {
    api->display->setRefreshRate((float) renderer->refreshRate);
    api->display->setScale(renderer->scale);

    const char* err;
    font = api->graphics->loadFont(renderer->fontpath, &err);

    if (font == NULL)
        api->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, renderer->fontpath, err);
}


void renderer_draw(Renderer* renderer, PlaydateAPI* api) {
    const struct playdate_graphics* graphics = api->graphics;
    float angle = (api->system->getCrankAngle() / 360.0f) * 63.0f;

    uint8_t* data = graphics->getFrame();

    graphics->clear(kColorWhite);

    renderer_draw_line(data, 0, 0, 100, 20);

    for (int rowIndex = 0; rowIndex < renderer->rows; rowIndex++) {
        for (int columnIndex = 0; columnIndex < renderer->columns; columnIndex++) {
            int byteIndex = calculate_byte_index(rowIndex, columnIndex);

            // Get the corresponding value from the Bayer matrix
//            int bayer = bayer_value(rowIndex, columnIndex, BAYER_TABLE);
//
//            // TODO: Implement a real way to get the brightness of a pixel.
//            float pixelBrightness = angle;
//
//            if (pixelBrightness > (float) bayer) {
//                set_pixel_on(data, byteIndex, columnIndex);
//            } else {
//                set_pixel_off(data, byteIndex, columnIndex);
//            }
        }
    }
}

/**
* @brief Draws a line on the given frame.
*
* This function draws a line on the given frame using Bresenham's line algorithm.
*
* @param frame Pointer to the frame buffer.
* @param x1    The x-coordinate of the starting point of the line.
* @param y1    The y-coordinate of the starting point of the line.
* @param x2    The x-coordinate of the ending point of the line.
* @param y2    The y-coordinate of the ending point of the line.
*/

void renderer_draw_line(uint8_t* frame, int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;
    int e2;

    while (true) {
        int columnIndex = x1 % 8;
        int byteIndex = calculate_byte_index(y1, x1);
        set_pixel_off(frame, byteIndex, columnIndex);

        if (x1 == x2 && y1 == y2) {
            break;
        }
        e2 = err;
        if (e2 > -dx) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dy) {
            err += dx;
            y1 += sy;
        }
    }
}

/**
 * @brief Sets a specific bit in a byte of data to 0.
 *
 * This function modifies a specific bit in the given byte of data to 0.
 * The byteIndex parameter determines the index of the byte in the data array to modify.
 * The columnIndex parameter determines the bit position (0-7) within the byte to modify.
 *
 * @param*/

void set_pixel_off(uint8_t* data, int byteIndex, int columnIndex) {
    data[byteIndex] &= ~(1 << (7 - (columnIndex % 8)));
}

/**
 * @brief Sets a pixel to ON in the data array.
 *
 * This function sets a pixel to ON (1) in the data array at the specified byte index and column index.
 * The data array is modified in-place.
 *
 * @param data The data array to modify.
 * @param byteIndex The index of the byte in the data array.
 * @param columnIndex The index of the column within the byte.
 *
 * @return None.
 */

void set_pixel_on(uint8_t* data, int byteIndex, int columnIndex) {
    data[byteIndex] |= (1 << (7 - (columnIndex % 8)));
}

/**
 * @brief Calculates the byte index based on the row index and column index.
 *
 * This function takes the row index and column index as input and calculates
 * the corresponding byte index based on a pre-defined LCD row size and the
 * assumption that each byte holds 8 columns.
 *
 * @param rowIndex The index of the desired row.
 * @param columnIndex The index of the desired column.
 * @return The byte index calculated based on the row and column indices.
 */

int calculate_byte_index(int rowIndex, int columnIndex) {
    return rowIndex * LCD_ROWSIZE + columnIndex / 8;
}

void renderer_cleanup(Renderer* renderer) {
    // Cleanup resources and memory used by the renderer
    // ...
}