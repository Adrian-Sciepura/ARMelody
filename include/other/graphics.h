#ifndef OTHER_GRAPHICS_H_
#define OTHER_GRAPHICS_H_

#include <stdint.h>

typedef struct
{
    uint8_t* pixels;
    uint32_t width;
    uint32_t height;
    uint32_t bytes_per_pixel;
} graphics_buffer_t;

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} graphics_color24_t;

void graphics_buffer_create(graphics_buffer_t* graphics_buffer, uint32_t width, uint32_t height, uint32_t bytes_per_pixel);
void graphics_buffer_free(graphics_buffer_t* graphics_buffer);

graphics_color24_t graphics_get_pixel(graphics_buffer_t* graphics_buffer, uint32_t x, uint32_t y);
void graphics_draw_pixel(graphics_buffer_t* graphics_buffer, uint32_t x, uint32_t y, graphics_color24_t color);
void graphics_draw_line(graphics_buffer_t* graphics_buffer, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t thickness, graphics_color24_t color);
void graphics_draw_rect(graphics_buffer_t* graphics_buffer, uint32_t x, uint32_t y, uint32_t w, uint32_t h, graphics_color24_t color);
void graphics_draw_buffer(graphics_buffer_t* graphics_buffer, uint32_t x, uint32_t y, graphics_buffer_t* other_buffer, uint32_t xo, uint32_t yo, uint32_t wo, uint32_t ho);

#endif