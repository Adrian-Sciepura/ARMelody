#include <files/bmp.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static const uint16_t BMP_BM_MAGIC = 0x4D42;
static const uint32_t BMP_PIXEL_ARRAY_OFFSET = sizeof(bmp_file_header_t) + sizeof(bmp_info_header_t);

void bmp_create(bmp_image_t* bmp_image, uint32_t width, uint32_t height, uint32_t bits_per_pixel)
{
    graphics_buffer_create(&bmp_image->graphics_buffer, width, height, bits_per_pixel / 8);
    int real_row_size = ((bits_per_pixel / 8) * width + 3) & ~3;
    int data_size = real_row_size * height;

    bmp_image->info_header = (bmp_info_header_t)
    {
        .header_size = 40,
        .image_width = width,
        .image_height = height,
        .number_of_color_planes = 1,
        .bits_per_pixel = bits_per_pixel,
        .compression_method = BMP_COMPRESSION_METHOD_RGB,
        .image_size = data_size,
        .horizontal_resolution = 0,
        .vertical_resolution = 0,
        .number_of_colors = 0,
        .number_of_important_colors = 0
    };
}

void bmp_free(bmp_image_t* bmp_image)
{
    graphics_buffer_free(&bmp_image->graphics_buffer);
    memset(bmp_image, 0, sizeof(bmp_image_t));
}

graphics_buffer_t bmp_detach_buffer(bmp_image_t* bmp_image)
{
    graphics_buffer_t buffer = bmp_image->graphics_buffer;
    memset(&bmp_image->graphics_buffer, 0, sizeof(graphics_buffer_t));
    return buffer;
}

bmp_status_t bmp_load(const char* filename, bmp_image_t* bmp_image)
{
    FILE* bmp_file = fopen(filename, "rb");

    if(!bmp_file)
    {
        return BMP_ERROR_UNABLE_TO_OPEN_FILE;
    }

    bmp_file_header_t file_header;
    if(!fread(&file_header, sizeof(bmp_file_header_t), 1, bmp_file))
    {
        fclose(bmp_file);
        return BMP_ERROR_FILE_HEADER_LENGTH_CORRUPTED;
    }

    uint32_t info_header_type = 0;
    if(!fread(&info_header_type, 4, 1, bmp_file))
    {
        fclose(bmp_file);
        return BMP_ERROR_INFO_HEADER_LENGTH_CORRUPTED;
    }

    if(info_header_type != 40)
    {
        fclose(bmp_file);
        return BMP_ERROR_INFO_HEADER_UNSUPPORTED_TYPE;
    }

    if(!fread(((uint8_t*)&bmp_image->info_header) + 4, sizeof(bmp_info_header_t) - 4, 1, bmp_file))
    {
        fclose(bmp_file);
        return BMP_ERROR_INFO_HEADER_LENGTH_CORRUPTED;
    }

    if(bmp_image->info_header.bits_per_pixel != 24)
    {
        fclose(bmp_file);
        return BMP_ERROR_INFO_HEADER_UNSUPPORTED_BITS_PER_PIXEL;
    }

    bmp_image->info_header.header_size = 40;

    if(fseek(bmp_file, file_header.offset, SEEK_SET))
    {
        fclose(bmp_file);
        return BMP_ERROR_DATA_POSITION_CORRUPTED;
    }

    int width = bmp_image->info_header.image_width;
    int height = bmp_image->info_header.image_height;
    int bits_per_pixel = bmp_image->info_header.bits_per_pixel;
    int bytes_per_pixel = bits_per_pixel / 8;
    int real_row_size = ((bits_per_pixel / 8) * width + 3) & ~3;
    graphics_buffer_create(&bmp_image->graphics_buffer, width, height, bits_per_pixel / 8);

    uint8_t* buffer = (uint8_t*)malloc(real_row_size);
    for(int row = 0; row < height; row++)
    {
        uint8_t* current_line = &bmp_image->graphics_buffer.pixels[(height - row - 1) * width * bytes_per_pixel];
        if(fread(buffer, 1, real_row_size, bmp_file) != real_row_size)
        {
            free(buffer);
            graphics_buffer_free(&bmp_image->graphics_buffer);
            fclose(bmp_file);
            return BMP_ERROR_DATA_LENGTH_CORRUPTED;
        }

        for(int col = 0; col < width * bytes_per_pixel; col += bytes_per_pixel)
        {
            for(int byte = 0; byte < bytes_per_pixel; byte++)
            {
                current_line[col + byte] = buffer[col + (bytes_per_pixel - byte - 1)];
            }
        }
    }

    free(buffer);
    fclose(bmp_file);
    return BMP_OK;
}

bmp_status_t bmp_save(const char* filename, bmp_image_t* bmp_image)
{
    FILE* bmp_file = fopen(filename, "wb");

    if(!bmp_file)
    {
        return BMP_ERROR_UNABLE_TO_SAVE_FILE;
    }

    uint32_t file_size = BMP_PIXEL_ARRAY_OFFSET + bmp_image->info_header.image_size;

    bmp_file_header_t file_header = 
    {
        .type = BMP_BM_MAGIC,
        .size = file_size,
        .reserved1 = 0,
        .reserved2 = 0,
        .offset = BMP_PIXEL_ARRAY_OFFSET
    };

    if(!fwrite(&file_header, sizeof(bmp_file_header_t), 1, bmp_file))
    {
        fclose(bmp_file);
        return BMP_ERROR_UNABLE_TO_SAVE_FILE;
    }

    if(!fwrite(&bmp_image->info_header, sizeof(bmp_info_header_t), 1, bmp_file))
    {
        fclose(bmp_file);
        return BMP_ERROR_UNABLE_TO_SAVE_FILE;
    }
    
    uint32_t height = bmp_image->info_header.image_height;
    uint32_t width = bmp_image->info_header.image_width;
    uint32_t bits_per_pixel = bmp_image->info_header.bits_per_pixel;
    uint32_t bytes_per_pixel = bits_per_pixel / 8;
    uint32_t real_row_size = (bytes_per_pixel * width + 3) & ~3;
    uint32_t padding = real_row_size - width * bytes_per_pixel;
    
    uint8_t* buffer = (uint8_t*)malloc(real_row_size);
    for(int row = 0; row < height; row++)
    {
        uint8_t* current_line = &bmp_image->graphics_buffer.pixels[(height - row - 1) * width * bytes_per_pixel];
        for(int col = 0; col < width * bytes_per_pixel; col += bytes_per_pixel)
        {
            for(int byte = 0; byte < bytes_per_pixel; byte++)
            {
                buffer[col + byte] = current_line[col + (bytes_per_pixel - byte - 1)];
            }
        }

        for(int pad = 0; pad < padding; pad++)
        {
            buffer[real_row_size - pad - 1] = 0;
        }
        
        if(fwrite(buffer, 1, real_row_size, bmp_file) != real_row_size)
        {
            free(buffer);
            fclose(bmp_file);
            return BMP_ERROR_UNABLE_TO_SAVE_FILE;
        }
    }

    free(buffer);
    fclose(bmp_file);
    return BMP_OK;
}