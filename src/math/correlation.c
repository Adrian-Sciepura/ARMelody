#include <math/correlation.h>
#include <math/fft.h>
#include <math/consts.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void correlation(complex_t* orginal_data, complex_t* potential_match_data, int n)
{
    int padded_size = 2;
    while(n > padded_size)
        padded_size *= 2;

    printf("Padded size: %d\n", padded_size);

    complex_t* padded_orginal = (complex_t*)calloc(padded_size, sizeof(complex_t));
    complex_t* padded_potential = (complex_t*)calloc(padded_size, sizeof(complex_t));

    for(int i = 0; i < n; i++)
    {
        padded_orginal[i] = orginal_data[i];
        padded_potential[i] = potential_match_data[i];
    }
    // result = IFFT(FFT(padded_orginal) x conjugate(FFT(padded_potential)))
    clock_t start = clock();
    fft_iterative(padded_orginal, padded_size, false);
    clock_t fft_1 = clock();
    fft_iterative(padded_potential, padded_size, false);
    clock_t fft_2 = clock();
    // for(int i = 0; i < padded_size; i++)
    // {
    //    printf("padded_orginal[%d] = %f + %fi\n", i, padded_potential[i].re, padded_potential[i].im);
    // }
    
    for(int i = 0; i < padded_size; i++)
    {
        padded_potential[i].im = -padded_potential[i].im;
        complex_mul_inplace(&padded_potential[i], padded_orginal[i]);
    }
    // for(int i = 0; i < padded_size; i++)
    // {
    //    printf("padded_orginal[%d] = %f + %fi\n", i, padded_potential[i].re, padded_potential[i].im);
    // }
    clock_t fft_3 = clock();
    fft_iterative(padded_potential, padded_size, true);
    clock_t fft_4 = clock();
    // for(int i = 0; i < padded_size; i++)
    // {
    //    printf("padded_orginal[%d] = %f + %fi\n", i, padded_potential[i].re, padded_potential[i].im);
    // }

    printf("Time: %fs\n", (double)(fft_1 - start) / CLOCKS_PER_SEC);
    printf("Time: %fs\n", (double)(fft_2 - fft_1) / CLOCKS_PER_SEC);
    printf("Time: %fs\n", (double)(fft_4 - fft_3) / CLOCKS_PER_SEC);
    correlation_interpretation(padded_potential, padded_size);

    free(padded_orginal);
    free(padded_potential);
}

void correlation_interpretation(complex_t* data, int n)
{
    int best_index = 0;
    float max_magnitude = 0;
    for(int i = 0; i < n; i++)
    {
        float magnitude = sqrt(data[i].re * data[i].re + data[i].im * data[i].im);
        if(magnitude > max_magnitude)
        {
            max_magnitude = magnitude;
            best_index = i;
        }
    }
    printf("Best match found at index %d with magnitude %f\n", best_index, max_magnitude);
}

void correlation_neon(complex_t* orginal_data, complex_t* potential_match_data, int n)
{
    int padded_size = 2;
    while(n > padded_size)
        padded_size *= 2;

    printf("Padded size: %d\n", padded_size);
        
    complex_t* padded_orginal = (complex_t*)calloc(padded_size, sizeof(complex_t));
    complex_t* padded_potential = (complex_t*)calloc(padded_size, sizeof(complex_t));

    for(int i = 0; i < n; i++)
    {
        padded_orginal[i] = orginal_data[i];
        padded_potential[i] = potential_match_data[i];
    }
    // result = IFFT(FFT(padded_orginal) x conjugate(FFT(padded_potential)))
    fft_iterative_neon_fixed(padded_orginal, padded_size, false);
    fft_iterative_neon_fixed(padded_potential, padded_size, false);
    

    // for(int i = 0; i < padded_size; i++)
    // {
    //    printf("padded_orginal[%d] = %f + %fi\n", i, padded_potential[i].re, padded_potential[i].im);
    // }
    
    for(int i = 0; i < padded_size; i++)
    {
        padded_potential[i].im = -padded_potential[i].im;
        complex_mul_inplace(&padded_potential[i], padded_orginal[i]);
    }
    // for(int i = 0; i < padded_size; i++)
    // {
    //    printf("padded_orginal[%d] = %f + %fi\n", i, padded_potential[i].re, padded_potential[i].im);
    // }
    fft_iterative_neon_fixed(padded_potential, padded_size, true);
    // for(int i = 0; i < padded_size; i++)
    // {
    //    printf("padded_orginal[%d] = %f + %fi\n", i, padded_potential[i].re, padded_potential[i].im);
    // }
    correlation_interpretation(padded_potential, padded_size);

    free(padded_orginal);
    free(padded_potential);
}

void correlation_interpretation_neon(complex_neon_t* data, int n)
{
    int best_index = 0;
    float32x4_t max_magnitude = vdupq_n_f32(0);
    for(int i = 0; i < n; i++)
    {
        float32x4_t magnitude = vaddq_f32(vmulq_f32(data[i].re, data[i].re), vmulq_f32(data[i].im, data[i].im));
        max_magnitude = vmaxq_f32(max_magnitude, magnitude);
    }
    float max_val[4];
    vst1q_f32(max_val, max_magnitude);
    printf("Best match found at index %d with magnitude %f\n", best_index, max_val[0]);
}