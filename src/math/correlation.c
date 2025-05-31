#include <math/correlation.h>
#include <math/consts.h>
#include <other/simple_timer.h>
#include <stdlib.h>
#include <stdio.h>

#define MEASURE_TIME(func, time_var)                        \
    simple_timer_start(&time_var);                          \
    func;                                                   \
    simple_timer_stop(&time_var);                           \
    printf("Time: %fs\n", simple_timer_get_time(&time_var));

void correlation(complex_t* orginal_data, complex_t* potential_match_data, int n, fft_iterative_callback fft_iter_func)
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
    simple_timer_t timer;
    
    // result = IFFT(FFT(padded_orginal) x conjugate(FFT(padded_potential)))
    MEASURE_TIME(fft_iter_func(padded_orginal, padded_size, false), timer);
    MEASURE_TIME(fft_iter_func(padded_potential, padded_size, false), timer);

    for(int i = 0; i < padded_size; i++)
    {
        padded_potential[i].im = -padded_potential[i].im;
        complex_mul_inplace(&padded_potential[i], padded_orginal[i]);
    }

    MEASURE_TIME(fft_iter_func(padded_potential, padded_size, true), timer);
    
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
