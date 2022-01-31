/// @file gfx.c
/// @author Florent Gluck
/// @date November 6, 2016
/// Helper routines to render pixels in fullscreen graphic mode.
/// Uses the SDL2 library.

#include "gfx.h"
#include "../geom/utils.h"
#include <assert.h>

static int flip_vertical(int iy, int height) {
    // assert(height >= iy);
    if (height < iy) {
        printf("height = %d, iy = %d\n", height, iy);
        exit(1);
    };
    return height - iy;
}

/// Create a fullscreen graphic window.
/// @param title Title of the window.
/// @param width Width of the window in pixels.
/// @param height Height of the window in pixels.
/// @return a pointer to the graphic context or NULL if it failed.
struct gfx_context_t *gfx_create(char *title, uint width, uint height) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        goto error;
    SDL_Window *window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED, width,
                                          height, SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture *texture =
        SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                          SDL_TEXTUREACCESS_STREAMING, width, height);
    uint32_t *pixels = malloc(width * height * sizeof(uint32_t));
    struct gfx_context_t *ctxt = malloc(sizeof(struct gfx_context_t));

    if (!window || !renderer || !texture || !pixels || !ctxt)
        goto error;

    ctxt->renderer = renderer;
    ctxt->texture = texture;
    ctxt->window = window;
    ctxt->width = width;
    ctxt->height = height;
    ctxt->pixels = pixels;

    SDL_ShowCursor(SDL_DISABLE);
    gfx_clear(ctxt, COLOR_BLACK);
    return ctxt;

error:
    return NULL;
}

/// Draw a pixel in the specified graphic context.
/// @param ctxt Graphic context where the pixel is to be drawn.
/// @param x X coordinate of the pixel.
/// @param y Y coordinate of the pixel.
/// @param color Color of the pixel.
void gfx_putpixel(struct gfx_context_t *ctxt, int x, int y, uint32_t color) {
    if (x < ctxt->width && y < ctxt->height)
        ctxt->pixels[ctxt->width * y + x] = color;
}

/// Clear the specified graphic context.
/// @param ctxt Graphic context to clear.
/// @param color Color to use.
void gfx_clear(struct gfx_context_t *ctxt, uint32_t color) {
    int n = ctxt->width * ctxt->height;
    while (n)
        ctxt->pixels[--n] = color;
}

/// Display the graphic context.
/// @param ctxt Graphic context to clear.
void gfx_present(struct gfx_context_t *ctxt) {
    SDL_UpdateTexture(ctxt->texture, NULL, ctxt->pixels,
                      ctxt->width * sizeof(uint32_t));
    SDL_RenderCopy(ctxt->renderer, ctxt->texture, NULL, NULL);
    SDL_RenderPresent(ctxt->renderer);
}

/// Destroy a graphic window.
/// @param ctxt Graphic context of the window to close.
void gfx_destroy(struct gfx_context_t *ctxt) {
    SDL_ShowCursor(SDL_ENABLE);
    SDL_DestroyTexture(ctxt->texture);
    SDL_DestroyRenderer(ctxt->renderer);
    SDL_DestroyWindow(ctxt->window);
    free(ctxt->pixels);
    ctxt->texture = NULL;
    ctxt->renderer = NULL;
    ctxt->window = NULL;
    ctxt->pixels = NULL;
    SDL_Quit();
    free(ctxt);
}

/// If a key was pressed, returns its key code (non blocking call).
/// List of key codes: https://wiki.libsdl.org/SDL_Keycode
/// @return the key that was pressed or 0 if none was pressed.
SDL_Keycode gfx_keypressed() {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
        if (event.type == SDL_KEYDOWN)
            return event.key.keysym.sym;
    }
    return 0;
}

void gfx_draw_circle(struct gfx_context_t *ctxt, vec pos, double r,
                     uint32_t color, double x0, double x1, double y0,
                     double y1) {
    // copy paste from the algo
    // https://fr.wikipedia.org/wiki/Algorithme_de_trac%C3%A9_d%27arc_de_cercle_de_Bresenham
    int width = ctxt->width;
    int height = ctxt->height;

    int rescaled_x = (int)rescale_to_window(0, width, x0, x1, pos.x);
    int rescaled_y = (int)rescale_to_window(0, height, y0, y1, pos.y);
    int rescaled_radius = (int)rescale_to_window(0, width, x0, x1, r);

    int y = rescaled_radius;
    int m = 3 - 2 * rescaled_radius;
    for (int x = 0; x <= y; ++x) {
        if (m < 0) {
            m += 4 * x + 6;
        } else {
            y -= 1;
            m += (4 * (x - y) + 10);
        }

        gfx_putpixel(ctxt, (x + rescaled_x + width) % width,
                     flip_vertical((y + rescaled_y + height) % height, height),
                     color);
        gfx_putpixel(ctxt, (y + rescaled_x + width) % width,
                     flip_vertical((x + rescaled_y + height) % height, height),
                     color);
        gfx_putpixel(ctxt, (-x + rescaled_x + width) % width,
                     flip_vertical((y + rescaled_y + height) % height, height),
                     color);
        gfx_putpixel(ctxt, (-y + rescaled_x + width) % width,
                     flip_vertical((x + rescaled_y + height) % height, height),
                     color);
        gfx_putpixel(ctxt, (x + rescaled_x + width) % width,
                     flip_vertical((-y + rescaled_y + height) % height, height),
                     color);
        gfx_putpixel(ctxt, (y + rescaled_x + width) % width,
                     flip_vertical((-x + rescaled_y + height) % height, height),
                     color);
        gfx_putpixel(ctxt, (-x + rescaled_x + width) % width,
                     flip_vertical((-y + rescaled_y + height) % height, height),
                     color);
        gfx_putpixel(ctxt, (-y + rescaled_x + width) % width,
                     flip_vertical((-x + rescaled_y + height) % height, height),
                     color);
    }
}

extern void gfx_draw_line(struct gfx_context_t *ctxt, int x0, int x1, int y0,
                          int y1, uint32_t color) {
    int width = ctxt->width;
    int height = ctxt->height;
    // copy pasta from
    // <https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm>
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy; /* error value e_xy */
    while (true) {     /* loop */
        int pos_x = (x0 + width) % width;
        int pos_y = (y0 + height) % height;
        gfx_putpixel(ctxt, pos_x, flip_vertical(pos_y, height), color);
        if (x0 == x1 && y0 == y1)
            break;
        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy; /* e_xy+e_x > 0 */
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx; /* e_xy+e_y < 0 */
            y0 += sy;
        }
    }
}

extern void gfx_draw_triangle(struct gfx_context_t *ctxt, triangle t,
                              uint32_t color, double x0, double x1, double y0,
                              double y1) {
    int width = ctxt->width;
    int height = ctxt->height;

    int v1_rescaled_x = (int)rescale_to_window(0, width, x0, x1, t.v1.x);
    int v1_rescaled_y = (int)rescale_to_window(0, height, y0, y1, t.v1.y);

    int v2_rescaled_x = (int)rescale_to_window(0, width, x0, x1, t.v2.x);
    int v2_rescaled_y = (int)rescale_to_window(0, height, y0, y1, t.v2.y);

    int v3_rescaled_x = (int)rescale_to_window(0, width, x0, x1, t.v3.x);
    int v3_rescaled_y = (int)rescale_to_window(0, height, y0, y1, t.v3.y);

    gfx_draw_line(ctxt, v1_rescaled_x, v3_rescaled_x, v1_rescaled_y,
                  v3_rescaled_y, color);
    gfx_draw_line(ctxt, v2_rescaled_x, v3_rescaled_x, v2_rescaled_y,
                  v3_rescaled_y, color);
    gfx_draw_line(ctxt, v2_rescaled_x, v1_rescaled_x, v2_rescaled_y,
                  v1_rescaled_y, color);

    vec centroid = triangle_center_of_gravity(t);
    int centroid_x = (int)rescale_to_window(0, width, x0, x1, centroid.x);
    int centroid_y = (int)rescale_to_window(0, height, y0, y1, centroid.y);

    gfx_putpixel(ctxt, centroid_x, flip_vertical(centroid_y, height), color);
}

void gfx_draw_dot(struct gfx_context_t *ctxt, vec pos, uint32_t color,
                  double x0, double x1, double y0, double y1) {
    int width = ctxt->width;
    int height = ctxt->height;

    int pos_x = (int)rescale_to_window(0, width, x0, x1, pos.x);
    int pos_y = (int)rescale_to_window(0, height, y0, y1, pos.y);

    gfx_putpixel(ctxt, pos_x, flip_vertical(pos_y, height), color);
}

actions gfx_interpret_key(SDL_Keycode key) {
    switch (key) {
    case SDLK_SPACE:
        return fire_bullet;
        break;
    case SDLK_RIGHT:
        return turn_right;
        break;
    case SDLK_LEFT:
        return turn_left;
        break;
    case SDLK_UP:
        return front_boost;
        break;
    case SDLK_DOWN:
        return rear_boost;
        break;
    case SDLK_ESCAPE:
        return exit_game;
        break;
    default:
        return no_action;
        break;
    }
}
