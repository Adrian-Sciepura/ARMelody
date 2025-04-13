#include <math/correlation.h>
#include <math/fft.h>
#include <math/consts.h>
#include <stdlib.h>
#include <stdio.h>

void correlation(complex_t* orginal_data, complex_t* potential_match_data, int n)
{
    complex_t* orginal_data_copy = (complex_t*)malloc(n * sizeof(complex_t));
    for(int i = 0; i < n; i++)
        orginal_data_copy[i] = orginal_data[i];

    // result = IFFT(FFT(orginal_data_copy) x conjugate(FFT(potential_match_data)))
    cooley_tukey_fft(orginal_data_copy, n);

    cooley_tukey_fft(potential_match_data, n);

    for(int i = 0; i < n; i++)
        potential_match_data[i].im = -potential_match_data[i].im;
        

    for(int i = 0; i < n; i++)
        complex_mul_inplace(potential_match_data[i], orginal_data_copy[i]);

    cooley_tukey_ifft(potential_match_data, n);

    free(orginal_data_copy);
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