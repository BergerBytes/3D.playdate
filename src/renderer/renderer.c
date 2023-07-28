//
// Created by Michael Berger on 7/14/23.
//

#include "renderer.h"
#include "bayer.h"
#include "mesh.h"

LCDFont* font = NULL;

const int BAYER_TABLE = BAYER_8;
const float BAYER_MULTIPLIER = 64;

Mesh mesh;

void set_pixel_on(uint8_t* data, int byteIndex, int columnIndex);

void set_pixel_off(uint8_t* data, int byteIndex, int columnIndex);

int calculate_byte_index(int rowIndex, int columnIndex);

void renderer_draw_line(uint8_t* frame, int x1, int y1, int x2, int y2, int frame_width, int frame_height, int color);

void renderer_draw_line_by_vectors(uint8_t* data, Vector3 v1, Vector3 v2, int frame_width, int frame_height, int color);

void renderer_draw_line_by_triangle(uint8_t* data, Triangle triangle, int frame_width, int frame_height, int color);

void renderer_draw_fill(
        uint8_t* frame,
        int x1, int y1,
        int x2, int y2,
        int x3, int y3,
        int frame_width, int frame_height,
        float brightness
);

void renderer_draw_fill_by_triangle(
        uint8_t* data,
        Triangle triangle, int frame_width, int frame_height,
        float brightness
);

void renderer_draw_normal(Renderer* renderer, uint8_t* data, Triangle triangle, int color);

Vector3 renderer_transform_to_2d_space(Renderer* renderer, Vector3* vector);

int min(int a, int b);

int max(int a, int b);

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
    renderer->cameraPosition = (Vector3) {.x = 0.0f, .y = 0.0f, .z = 0.0f};
    renderer->projectionMatrix = matrix4X4_projection(
            60,
            (float) renderer->columns / (float) renderer->rows,
            0.1f,
            100.0f
    );
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

    mesh = create_cube_mesh();
}

float last_angle = 400.0f;

void renderer_draw(Renderer* renderer, PlaydateAPI* api) {
    const struct playdate_graphics* graphics = api->graphics;
    float angle = api->system->getCrankAngle();
    if (angle == last_angle)
        return;
    last_angle = angle;

    float theta_radians = angle * PI / 180.0f;

    Matrix4x4 rotationX = {
            .m = {
                    {1.0f, 0.0f,                0.0f,                 0.0f},
                    {0.0f, cosf(theta_radians), -sinf(theta_radians), 0.0f},
                    {0.0f, sinf(theta_radians), cosf(theta_radians),  0.0f},
                    {0.0f, 0.0f,                0.0f,                 1.0f}
            }
    };

    Matrix4x4 rotationZ = {
            .m = {
                    {cosf(theta_radians), -sinf(theta_radians), 0.0f, 0.0f},
                    {sinf(theta_radians), cosf(theta_radians),  0.0f, 0.0f},
                    {0.0f,                0.0f,                 1.0f, 0.0f},
                    {0.0f,                0.0f,                 0.0f, 1.0f}
            }
    };

    Matrix4x4 rotationY = {
            .m = {
                    {cosf(theta_radians),  0.0f, sinf(theta_radians), 0.0f},
                    {0.0f,                 1.0f, 0.0f,                0.0f},
                    {-sinf(theta_radians), 0.0f, cosf(theta_radians), 0.0f},
                    {0.0f,                 0.0f, 0.0f,                1.0f}
            }
    };

    Matrix4x4 rotation;
    matrix4x4_multiply(&rotationX, &rotationZ, &rotation);
    matrix4x4_multiply(&rotation, &rotationY, &rotation);

    uint8_t* data = graphics->getFrame();

    graphics->clear(kColorBlack);

    // For each triangle in mesh
    for (int i = 0; i < mesh.triangleCount; i++) {
        Triangle triangleTranslated = mesh.triangles[i];
        Triangle triangleRotate = mesh.triangles[i];
        Triangle triangleProjected = mesh.triangles[i];

        // Apply rotation to triangle
        for (int p = 0; p < 3; p++) {
            vector3_multiply_matrix4x4(
                    &mesh.triangles[i].points[p],
                    &triangleRotate.points[p],
                    &rotation
            );
        }

        triangleTranslated = triangleRotate;

        // Move triangle away from camera
        for (int p = 0; p < 3; p++) {
            triangleTranslated.points[p].z += 3.0f;
        }

        float dot = vector3_dot_product(
                triangle_normal(&triangleTranslated),
                vector3_subtract(
                        triangleTranslated.points[0],
                        renderer->cameraPosition
                )
        );

        if (dot >= 0) {
            continue;
        }

        // Project triangle from 3D to 2D
        for (int p = 0; p < 3; p++) {
            vector3_multiply_matrix4x4(
                    &triangleTranslated.points[p],
                    &triangleProjected.points[p],
                    &renderer->projectionMatrix
            );

            renderer_transform_to_2d_space(renderer, &triangleProjected.points[p]);
        }

        float brightness = i / 11.0f;

        renderer_draw_fill_by_triangle(
                data,
                triangleProjected,
                renderer->columns,
                renderer->rows,
                brightness
        );

        renderer_draw_line_by_triangle(
                data,
                triangleProjected,
                renderer->columns,
                renderer->rows,
                brightness > 0.2f ? kColorBlack : kColorWhite
        );
    }

    api->graphics->markUpdatedRows(0, renderer->rows - 1);
}

/**
 * @brief Returns the minimum of two integers.
 *
 * This function takes two integers as input and returns the smaller
 * value among them. If both the given integers are equal, the function
 * returns either one of them.
 *
 * @param a The first integer to compare.
 * @param b The second integer to compare.
 *
 * @return The minimum of the two integers.
 */

inline int min(int a, int b) {
    return (a < b) ? a : b;
}

/**
 * @brief Returns the maximum of two integers.
 *
 * This function takes two integer arguments, 'a' and 'b', and returns the maximum value
 * between them. If 'a' is greater than 'b', 'a' is returned, otherwise 'b' is returned.
 *
 * @param a The first integer value.
 * @param b The second integer value.
 * @return The maximum value between 'a' and 'b'.
 */

inline int max(int a, int b) {
    return (a > b) ? a : b;
}

/**
 * Determines whether a point is inside a triangle.
 *
 * This function uses the Barycentric Coordinate System to check whether
 * the given point (x, y) is inside the triangle defined by the points
 * (x1, y1), (x2, y2), and (x3, y3).
 *
 * @param x1 The x-coordinate of the first point of the triangle.
 * @param y1 The y-coordinate of the first point of the triangle.
 * @param x2 The x-coordinate of the second point of the triangle.
 * @param y2 The y-coordinate of the second point of the triangle.
 * @param x3 The x-coordinate of the third point of the triangle.
 * @param y3 The y-coordinate of the third point of the triangle.
 * @param x The x-coordinate of the point to check.
 * @param y The y-coordinate of the point to check.
 *
 * @return True if the point is inside the triangle, false otherwise.
 */

bool point_in_triangle(int x1, int y1, int x2, int y2, int x3, int y3, int x, int y) {
    // Barycentric Coordinate System
    float denominator = ((y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3));
    float alpha = ((y2 - y3) * (x - x3) + (x3 - x2) * (y - y3)) / denominator;
    float beta = ((y3 - y1) * (x - x3) + (x1 - x3) * (y - y3)) / denominator;
    float gamma = 1.0f - alpha - beta;

    return alpha > 0 && beta > 0 && gamma > 0;
}

/**
 * @brief Renders a filled triangle onto a frame buffer.
 *
 * The function calculates the bounding box of the triangle defined by (x1, y1), (x2, y2),
 * and (x3, y3) vertices. It then calculates the Barycentric coordinates at the vertices.
 * If the triangle vertices are in clockwise order, the function flips the comparisons.
 * It iterates through each pixel within the bounding box and checks if the pixel is on or
 * inside all edges of the triangle using Barycentric coordinates.
 * If the pixel is inside the triangle, it sets it to off in the given frame buffer.
 *
 * @param frame Pointer to the frame buffer
 * @param x1 X-coordinate of the first vertex of the triangle
 * @param y1 Y-coordinate of the first vertex of the triangle
 * @param x2 X-coordinate of the second vertex of the triangle
 * @param y2 Y-coordinate of the second vertex of the triangle
 * @param x3 X-coordinate of the third vertex of the triangle
 * @param y3 Y-coordinate of the third vertex of the triangle
 * @param frame_width Width of the frame buffer
 * @param frame_height Height of the frame buffer
 */

void renderer_draw_fill(
        uint8_t* frame,
        int x1, int y1,
        int x2, int y2,
        int x3, int y3,
        int frame_width, int frame_height,
        float brightness
) {
    // Calculate bounding box of the triangle
    int xMin = min(min(x1, x2), x3), xMax = max(max(x1, x2), x3);
    int yMin = min(min(y1, y2), y3), yMax = max(max(y1, y2), y3);

    xMin = max(xMin, 0);
    xMax = min(xMax, frame_width - 1);

    yMin = min(yMin, 0);
    yMax = min(yMax, frame_height - 1);

    for (int y = yMin; y <= yMax; y++) {
        for (int x = xMin; x <= xMax; x++) {

            // If p is on or inside all edges, render pixel
            if (point_in_triangle(x1, y1, x2, y2, x3, y3, x, y)) {
                if (x >= 0 && x < frame_width && y >= 0 && y < frame_height) {
                    int rowIndex = y % frame_height;
                    int columnIndex = x % frame_width;
                    int byteIndex = calculate_byte_index(rowIndex, columnIndex);

                    if ((int) (brightness * BAYER_MULTIPLIER) > bayer_value(rowIndex, columnIndex, BAYER_TABLE))
                        set_pixel_on(frame, byteIndex, columnIndex);
                    else
                        set_pixel_off(frame, byteIndex, columnIndex);

                }
            }
        }
    }
}

/**

 * @brief Renders a filled triangle on a frame buffer using the given data.
 *
 * This function takes a frame buffer data and a triangle, and fills the triangle in the frame buffer.
 * The triangle is defined by three points, and the frame buffer is defined by its dimensions.
 *
 * @param data Pointer to the frame buffer data.
 * @param triangle The triangle that needs to be filled in the frame buffer.
 * @param frame_width Width of the frame buffer.
 * @param frame_height Height of the frame buffer.
 */

void
renderer_draw_fill_by_triangle(uint8_t* data, Triangle triangle, int frame_width, int frame_height, float brightness) {
    renderer_draw_fill(
            data,
            (int) roundf(triangle.points[0].x), (int) roundf(triangle.points[0].y),
            (int) roundf(triangle.points[1].x), (int) roundf(triangle.points[1].y),
            (int) roundf(triangle.points[2].x), (int) roundf(triangle.points[2].y),
            frame_width,
            frame_height,
            brightness
    );
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

void renderer_draw_line(uint8_t* frame, int x1, int y1, int x2, int y2, int frame_width, int frame_height, int color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;
    int e2;

    while (1) {
        if (x1 >= 0 && x1 < frame_width && y1 >= 0 && y1 < frame_height) {
            int columnIndex = x1 % 8;
            int byteIndex = calculate_byte_index(y1, x1);
            if (color == kColorWhite)
                set_pixel_on(frame, byteIndex, columnIndex);
            else
                set_pixel_off(frame, byteIndex, columnIndex);
        }

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
 * @brief Draw a line by two 3D vectors on a given 2D raster frame
 * @param data The raster data to draw on
 * @param v1 The first vector representing the starting point of the line
 * @param v2 The second vector representing the ending point of the line
 * @param frame_width The width of the raster frame
 * @param frame_height The height of the raster frame
 *
 * This function draws a line on a given raster frame by converting the 3D vectors
 * (v1 and v2) to 2D coordinate points, rounding them to integers, and passing them
 * to the renderer_draw_line function.
 */

void renderer_draw_line_by_vectors(
        uint8_t* data,
        Vector3 v1, Vector3 v2,
        int frame_width, int frame_height,
        int color
) {
    renderer_draw_line(
            data,
            (int) roundf(v1.x), (int) roundf(v1.y),
            (int) roundf(v2.x), (int) roundf(v2.y),
            frame_width,
            frame_height,
            color
    );
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

/**
 * Renders a triangle by drawing lines between its vertices.
 *
 * @param data The buffer to render the triangle on.
 * @param triangle The triangle to be rendered.
 */

void renderer_draw_line_by_triangle(uint8_t* data, Triangle triangle, int frame_width, int frame_height, int color) {
    renderer_draw_line_by_vectors(data, triangle.points[0], triangle.points[1], frame_width, frame_height, color);
    renderer_draw_line_by_vectors(data, triangle.points[1], triangle.points[2], frame_width, frame_height, color);
    renderer_draw_line_by_vectors(data, triangle.points[2], triangle.points[0], frame_width, frame_height, color);
}

/**
 * @brief Draws a normal vector onto the given renderer data.
 *
 * This function calculates the normal vector of a triangle and draws it onto the renderer's data.
 * The normal vector is calculated as the average of the triangle's vertex normals, scaled down by 0.25.
 * The normal vector is then translated to a position centered on the triangle's midpoint.
 * The start and end points of the normal vector are projected onto the renderer's projection matrix
 * and transformed to 2D space using the renderer's transformation functions.
 * Finally, a line is drawn between the start and end points with the specified color onto the renderer's data.
 *
 * @param renderer Pointer to the Renderer struct.
 * @param data Pointer to the renderer's data.
 * @param triangle The triangle for which the normal vector will be drawn.
 * @param color The color to use for drawing the normal vector.
 */

void renderer_draw_normal(Renderer* renderer, uint8_t* data, Triangle triangle, int color) {
    Vector3 normal = triangle_normal(&triangle);

    Vector3 scaledNormal = vector3_scalar_multiply(normal, 0.25f);

    Vector3 origin = vector3_midpoint(
            triangle.points[0],
            triangle.points[1],
            triangle.points[2]
    );
    Vector3 normalTranslated = vector3_add(origin, scaledNormal);

    Vector3 start = {0};
    Vector3 end = {0};

    vector3_multiply_matrix4x4(
            &origin,
            &start,
            &renderer->projectionMatrix
    );
    renderer_transform_to_2d_space(renderer, &start);

    vector3_multiply_matrix4x4(
            &normalTranslated,
            &end,
            &renderer->projectionMatrix
    );
    renderer_transform_to_2d_space(renderer, &end);

    renderer_draw_line_by_vectors(data, start, end, renderer->columns, renderer->rows, color);
}

/**
 * Transforms a 3D vector to 2D space based on the provided renderer.
 *
 * @param renderer The Renderer object used for the transformation.
 * @param vector The 3D vector to be transformed.
 * @return The transformed 3D vector in 2D space.
 */

Vector3 renderer_transform_to_2d_space(Renderer* renderer, Vector3* vector) {
    vector->x += 1.0f;
    vector->y += 1.0f;

    vector->x *= 0.5f * (float) renderer->columns;
    vector->y *= 0.5f * (float) renderer->rows;

    return *vector;
}

void renderer_cleanup(Renderer* renderer) {
    // Cleanup resources and memory used by the renderer
    // ...
}