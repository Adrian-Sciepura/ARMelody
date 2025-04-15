#include <other/graphics.h>
#include <stdlib.h>

void graphics_buffer_create(graphics_buffer_t* graphics_buffer, uint32_t width, uint32_t height, uint32_t bytes_per_pixel)
{
    *graphics_buffer = (graphics_buffer_t)
    {
        .width = width,
        .height = height,
        .bytes_per_pixel = bytes_per_pixel,
        .pixels = (uint8_t*)malloc(width * height * bytes_per_pixel)
    };
}

void graphics_buffer_free(graphics_buffer_t* graphics_buffer)
{
    if(graphics_buffer->pixels != NULL)
    {
        free(graphics_buffer->pixels);
        graphics_buffer->pixels = NULL;
    }
}

graphics_color24_t graphics_get_pixel(graphics_buffer_t* graphics_buffer, uint32_t x, uint32_t y)
{
    if(x < 0 || y < 0 || x > graphics_buffer->width || y > graphics_buffer->height)
    {
        return (graphics_color24_t){ .r = 0, .g = 0, .b = 0 };
    }

    return *(graphics_color24_t*)(graphics_buffer->pixels + ((x + y * graphics_buffer->width) * graphics_buffer->bytes_per_pixel));
}

void graphics_draw_pixel(graphics_buffer_t* graphics_buffer, uint32_t x, uint32_t y, graphics_color24_t color)
{
    if(x < 0 || y < 0 || x > graphics_buffer->width || y > graphics_buffer->height)
    {
        return;
    }

    *(graphics_color24_t*)(graphics_buffer->pixels + ((x + y * graphics_buffer->width) * graphics_buffer->bytes_per_pixel)) = color;
}

void graphics_draw_line(graphics_buffer_t* graphics_buffer, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t thickness, graphics_color24_t color)
{
    int dx =  abs (x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs (y2 - y1), sy = y1 < y2 ? 1 : -1; 
    int err = dx + dy, e2;

    int half_thickness = thickness % 2 == 0 ? thickness / 2 : thickness / 2 + 1;
    int from = thickness % 2 == 0 ? -half_thickness : -half_thickness + 1;
    int to = half_thickness;

    while(1)
    {  
        for(int i = from; i < to; i++)
        {
            graphics_draw_pixel(graphics_buffer, x1, y1 + i, color);
        }    
        
        if (x1 == x2 && y1 == y2) 
        {
            break;
        }
        
        e2 = 2 * err;
        
        if(e2 >= dy) 
        { 
            err += dy; 
            x1 += sx; 
        }
        
        if(e2 <= dx) 
        { 
            err += dx; 
            y1 += sy; 
        }
    }
}

void graphics_draw_rect(graphics_buffer_t* graphics_buffer, uint32_t x, uint32_t y, uint32_t w, uint32_t h, graphics_color24_t color)
{
    for(int xp = 0; xp < w; xp++)
    {
        for(int yp = 0; yp < h; yp++)
        {
            graphics_draw_pixel(graphics_buffer, x + xp, y + yp, color);
        }
    }
}

void graphics_draw_buffer(graphics_buffer_t* graphics_buffer, uint32_t x, uint32_t y, graphics_buffer_t* other_buffer, uint32_t xo, uint32_t yo, uint32_t wo, uint32_t ho)
{
    for(int xp = 0; xp < wo; xp++)
    {
        for(int yp = 0; yp < ho; yp++)
        {
            graphics_draw_pixel(graphics_buffer, x + xp, y + yp, graphics_get_pixel(other_buffer, xo + xp, yo + yp));
        }
    }
}

void graphics_create_font(graphics_font_t* graphics_font, graphics_buffer_t* font_source, uint32_t char_width, uint32_t char_height, uint32_t vertical_spacing, uint32_t horizontal_spacing, graphics_color24_t font_color, graphics_color24_t background_color)
{
    *graphics_font = (graphics_font_t)
    {
        .font_buffer = *font_source,
        .char_width = char_width,
        .char_height = char_height,
        .vertical_spacing = vertical_spacing,
        .horizontal_spacing = horizontal_spacing,
        .font_color = font_color,
        .background_color = background_color
    };
}

void graphics_draw_char(graphics_buffer_t* graphics_buffer, graphics_font_t* graphics_font, uint32_t x, uint32_t y, char c, graphics_color24_t color)
{
    uint8_t row = c / 16;
    uint8_t col = c % 16;
    for(int xp = 0; xp < graphics_font->char_width; xp++)
    {
        for(int yp = 0; yp < graphics_font->char_height; yp++)
        {
            graphics_color24_t pixel = graphics_get_pixel(
                &graphics_font->font_buffer, 
                xp + col * graphics_font->char_width + graphics_font->horizontal_spacing, 
                yp + row * graphics_font->char_height + graphics_font->vertical_spacing);
            
            if(graphics_color_equal(pixel, graphics_font->background_color))
            {
                continue;
            }
            
            if(graphics_color_equal(pixel, graphics_font->font_color))
            {
                graphics_draw_pixel(graphics_buffer, x + xp, y + yp, color);
            }
        }
    }
}

void graphics_draw_string(graphics_buffer_t* graphics_buffer, graphics_font_t* graphics_font, uint32_t x, uint32_t y, char* string, graphics_color24_t color)
{
    int i = 0;
    int offset_x = 0;
    int offset_y = 0;

    while(string[i] != '\0')
    {
        if(string[i] == '\n')
        {
            offset_y += graphics_font->char_height;
            offset_x = 0;
            i++;
            continue;
        }

        graphics_draw_char(graphics_buffer, graphics_font, x + offset_x, y + offset_y, string[i], color);
        offset_x += graphics_font->char_width;
        i++;
    }
}