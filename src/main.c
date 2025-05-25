#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <files/wav.h>
#include <math/complex.h>
#include <math/fft.h>
#include <math/correlation.h>

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

    int number_of_samples = wav_file.header.data_size / (wav_file.header.bits_per_sample / 8);
    uint8_t* data = wav_file.content.data;
    complex_t* data_as_complex = (complex_t*)aligned_alloc(16, number_of_samples * sizeof(complex_t));

    switch(wav_file.header.bits_per_sample)
    {
        case 8:
            for(int i = 0; i < number_of_samples; i++)
                data_as_complex[i].re = (float)(data[i]);
        break;

        case 16:
            for(int i = 0; i < number_of_samples; i++)
                data_as_complex[i].re = (float)(((uint16_t*)data)[i]);
        break;

        case 24:
            for(int i = 0; i < number_of_samples; i++)
                data_as_complex[i].re = (float)(*(uint32_t*)(data + i * 3) & 0x00FFFFFF);

        case 32:
            for(int i = 0; i < number_of_samples; i++)
                data_as_complex[i].re = (float)(((uint32_t*)data)[i]);
        break;

        default:
            printf("Error bits per sample value: %d is incorrect\n", wav_file.header.bits_per_sample);
        break;
    }

    for(int i = 0; i < number_of_samples; i++)
        data_as_complex[i].im = 0;

    complex_t temp_data[12] = {
        { .re = 1, .im = 0 },
        { .re = 2, .im = 0 },
        { .re = 3, .im = 0 },
        { .re = 4, .im = 0 },
        { .re = 5, .im = 0 },
        { .re = 6, .im = 0 },
        { .re = 7, .im = 0 },
        { .re = 8, .im = 0 },
        { .re = 9, .im = 0 },
        { .re = 10, .im = 0 },
        { .re = 11, .im = 0 },
        { .re = 12, .im = 0 }
    };
    complex_t temp_data2[12] = {
        { .re = 1, .im = 0 },
        { .re = 2, .im = 0 },
        { .re = 3, .im = 0 },
        { .re = 4, .im = 0 },
        { .re = 5, .im = 0 },
        { .re = 6, .im = 0 },
        { .re = 7, .im = 0 },
        { .re = 8, .im = 0 },
        { .re = 9, .im = 0 },
        { .re = 10, .im = 0 },
        { .re = 11, .im = 0 },
        { .re = 12, .im = 0 }
    };
    //int number_of_samples = 12;
    correlation(data_as_complex, data_as_complex, number_of_samples);
    correlation_neon(data_as_complex, data_as_complex, number_of_samples);
    // complex_neon_t* data_as_complex_neon = (complex_neon_t*)malloc(number_of_samples / 4 * sizeof(complex_neon_t));
    // for(int i = 0; i < number_of_samples; i++)
    // {
    //     data_as_complex_neon[i / 4].re[i % 4] = temp_data[i].re;
    //     data_as_complex_neon[i / 4].im[i % 4] = temp_data[i].im;
    // }
    //correlation_neon(data_as_complex_neon, data_as_complex_neon, number_of_samples / 4);
    //correlation_neon(temp_data, temp_data, number_of_samples);
    //correlation(temp_data2, temp_data2, number_of_samples);
    // float32x4_t* data_as_complex_neon = (float32x4_t*)malloc(number_of_samples / 4 * sizeof(float32x4_t));
    // for(int i = 0; i < number_of_samples; i++)
    // {
    //     data_as_complex_neon[i / 4][i % 4] = vdupq_n_f32(data_as_complex[i].re);
    // }
    // correlation_neon((complex_neon_t*)data_as_complex_neon, (complex_neon_t*)data_as_complex_neon, number_of_samples / 4);
    //free(data_as_complex_neon);
    
    free(data_as_complex);
    wav_close(&wav_file);
    return 0;
}