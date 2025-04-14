#ifndef FILES_BMP_H_
#define FILES_BMP_H_

#include <stdint.h>
#include <stdbool.h>
#include <other/graphics.h>

typedef enum
{
    BMP_OK,
    BMP_ERROR_UNABLE_TO_OPEN_FILE,
    BMP_ERROR_UNABLE_TO_SAVE_FILE,
    BMP_ERROR_FILE_HEADER_LENGTH_CORRUPTED,
    BMP_ERROR_INFO_HEADER_UNSUPPORTED_TYPE,
    BMP_ERROR_INFO_HEADER_UNSUPPORTED_BITS_PER_PIXEL,
    BMP_ERROR_INFO_HEADER_LENGTH_CORRUPTED,
    BMP_ERROR_DATA_POSITION_CORRUPTED,
    BMP_ERROR_DATA_LENGTH_CORRUPTED
} bmp_status_t;

typedef enum
{
    BMP_COMPRESSION_METHOD_RGB              = 0,
    BMP_COMPRESSION_METHOD_RLE8             = 1,
    BMP_COMPRESSION_METHOD_RLE4             = 2,
    BMP_COMPRESSION_METHOD_BITFIELDS        = 3,
    BMP_COMPRESSION_METHOD_JPEG             = 4,
    BMP_COMPRESSION_METHOD_PNG              = 5,
    BMP_COMPRESSION_METHOD_ALPHABITFIELDS   = 6,
    BMP_COMPRESSION_METHOD_CMYK             = 11,
    BMP_COMPRESSION_METHOD_CMYKRLE8         = 12,
    BMP_COMPRESSION_METHOD_CMYKRLE4         = 13
} bmp_compression_method_t;

typedef struct __attribute__((packed))
{
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
} bmp_file_header_t;

typedef struct __attribute__((packed))
{
    uint32_t header_size;
    int32_t image_width;
    int32_t image_height;
    uint16_t number_of_color_planes;
    uint16_t bits_per_pixel;
    uint32_t compression_method;
    uint32_t image_size;
    int32_t horizontal_resolution;
    int32_t vertical_resolution;
    uint32_t number_of_colors;
    uint32_t number_of_important_colors;
} bmp_info_header_t;

typedef struct
{
    bmp_info_header_t info_header;
    graphics_buffer_t graphics_buffer;
} bmp_image_t;


void bmp_create(bmp_image_t* bmp_image, uint32_t width, uint32_t height, uint32_t bits_per_pixel);
void bmp_free(bmp_image_t* bmp_image);
bmp_status_t bmp_load(const char* filename, bmp_image_t* bmp_image);
bmp_status_t bmp_save(const char* filename, bmp_image_t* bmp_image);

#endif