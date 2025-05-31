#ifndef MATH_FFT_H_
#define MATH_FFT_H_

#include<stdbool.h>
#include <math/complex.h>

typedef void(*fft_recursive_callback)(complex_t* data, int n);
typedef void(*fft_iterative_callback)(complex_t* data, int n, bool invert);

void cooley_tukey_fft(complex_t* data, int n);
void cooley_tukey_ifft(complex_t* data, int n);

void fft_iterative(complex_t* data, int n, bool invert);
extern void fft_iterative_neon(complex_t* data, int n, bool invert);

#endif