#ifndef MATH_FFT_H_
#define MATH_FFT_H_

#include <math/complex.h>

typedef void(*fft_callback)(complex_t* data, int n);

void cooley_tukey_fft(complex_t* data, int n);

#endif