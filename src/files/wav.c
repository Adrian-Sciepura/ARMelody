#include <files/wav.h>
#include <stdlib.h>

static const uint64_t RIFF_MAGIC    = 0x46464952;
static const uint64_t WAVE_MAGIC    = 0x45564157;
static const uint64_t FMT_MAGIC     = 0x20746D66;
static const uint64_t DATA_MAGIC    = 0x61746164;


wav_status_t wav_open(const char* filename, wav_file_t* wav_file)
{
    wav_file->file = fopen(filename, "rb");
    
    if(!wav_file->file) 
    {
        return WAV_ERROR_UNABLE_TO_OPEN_FILE;
    }

    wav_header_t* header = &wav_file->header;

    if(!fread(header, sizeof(wav_header_t), 1, wav_file->file))
    {
        fclose(wav_file->file);
        return WAV_ERROR_HEADER_LENGTH_CORRUPTED;
    }

    if(header->riff_magic != RIFF_MAGIC)
    {
        fclose(wav_file->file);
        return WAV_ERROR_HEADER_RIFF_CHUNK_CORRUPTED;
    }

    if(header->wave_magic != WAVE_MAGIC)
    {
        fclose(wav_file->file);
        return WAV_ERROR_HEADER_WAVE_CHUNK_CORRUPTED;
    }

    if( header->fmt_magic != FMT_MAGIC || 
        header->block_size != 0x10 ||
        header->bytes_per_second != header->frequency * header->bytes_per_block ||
        header->bytes_per_block != header->number_of_channels * header->bits_per_sample / 8)
    {
        fclose(wav_file->file);
        return WAV_ERROR_HEADER_FMT_CHUNK_CORRUPTED;
    }

    if(header->data_magic != DATA_MAGIC)
    {
        fclose(wav_file->file);
        return WAV_ERROR_HEADER_DATA_CHUNK_CORRUPTED;
    }

    return WAV_OK;
}

void wav_close(wav_file_t* wav_file)
{
    if(wav_file->file)
    {
        fclose(wav_file->file);
    }

    if(wav_file->content.data != NULL)
    {
        free(wav_file->content.data);
    }
}

wav_status_t wav_read_content(wav_file_t* wav_file)
{
    if(!wav_file->file)
    {
        return WAV_ERROR_FILE_CLOSED;
    }

    uint32_t length = wav_file->header.data_size;
    wav_file->content.length = length;
    wav_file->content.data = (uint8_t*)malloc(length);

    if(fread(wav_file->content.data, 1, length, wav_file->file) != length)
    {
        free(wav_file->content.data);
        wav_file->content.data = NULL;
        wav_file->content.length = 0;
        return WAV_ERROR_DATA_LENGTH_CORRUPTED;
    }

    return WAV_OK;
}