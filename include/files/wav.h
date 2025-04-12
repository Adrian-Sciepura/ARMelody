#ifndef FILES_WAV_H_
#define FILES_WAV_H_

#include <stdio.h>
#include <stdint.h>

typedef enum
{
    WAV_OK,
    WAV_ERROR_UNABLE_TO_OPEN_FILE,
    WAV_ERROR_HEADER_LENGTH_CORRUPTED,
    WAV_ERROR_HEADER_RIFF_CHUNK_CORRUPTED,
    WAV_ERROR_HEADER_WAVE_CHUNK_CORRUPTED,
    WAV_ERROR_HEADER_FMT_CHUNK_CORRUPTED,
    WAV_ERROR_HEADER_DATA_CHUNK_CORRUPTED,
    WAV_ERROR_FILE_CLOSED,
    WAV_ERROR_DATA_LENGTH_CORRUPTED
} wav_status_t;

typedef struct 
{
    uint32_t riff_magic;
    uint32_t file_size;
    uint32_t wave_magic;

    uint32_t fmt_magic;
    uint32_t block_size;
    uint16_t audio_format;
    uint16_t number_of_channels;
    uint32_t frequency;
    uint32_t bytes_per_second;
    uint16_t bytes_per_block;
    uint16_t bits_per_sample;

    uint32_t data_magic;
    uint32_t data_size;
} wav_header_t;

typedef struct 
{
    uint32_t length;
    uint8_t* data;
} wav_content_t;

typedef struct 
{
    FILE* file;
    wav_header_t header;
    wav_content_t content;
} wav_file_t;


wav_status_t wav_open(const char* filename, wav_file_t* wav_file);
void wav_close(wav_file_t* wav_file);
wav_status_t wav_read_content(wav_file_t* wav_file);

#endif
