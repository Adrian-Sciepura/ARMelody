#include <stdio.h>
#include <stdint.h>
#include <files/wav.h>

void print_wav_info(wav_file_t* wav_file)
{
    printf("-------- WAV FILE DATA --------\n");
    printf("- File size: %d\n",          wav_file->header.file_size);
    printf("- Block size: %d\n",         wav_file->header.block_size);
    printf("- Audio format: %d\n",       wav_file->header.audio_format);
    printf("- Number of channels: %d\n", wav_file->header.number_of_channels);
    printf("- Frequency: %d\n",          wav_file->header.frequency);
    printf("- Bytes per second: %d\n",   wav_file->header.bytes_per_second);
    printf("- Bytes per block: %d\n",    wav_file->header.bytes_per_block);
    printf("- Bits per sample: %d\n",    wav_file->header.bits_per_sample);
    printf("- Data size: %d\n",          wav_file->header.data_size);
    printf("-------------------------------\n");
}

int main(int argc, char** argv) 
{
    if(argc < 2)
    {
        printf("Error, you need to specify file location\n");
    }

    wav_file_t wav_file;
    wav_status_t status = wav_open(argv[1], &wav_file);

    if(status != WAV_OK)
    {
        printf("An error occurred during opening the file [Code: %d]\n", status);
        return -1;
    }

    printf("File successfully opened\n");
    print_wav_info(&wav_file);


    status = wav_read_content(&wav_file);
    if(status != WAV_OK)
    {
        printf("An error occured during reading content of the file [Code: %d]\n", status);
        wav_close(&wav_file);
        return -1;
    }

    printf("Content of the file successfully read\n");
    wav_close(&wav_file);
    return 0;
}