#ifndef MATH_CORELATION_H_
#define MATH_CORELATION_H_

#include <math/complex.h>
#include <math/fft.h>

void correlation(complex_t* orginal_data, complex_t* potential_match_data, int n, fft_iterative_callback fft_func);

void correlation_interpretation(complex_t* data, int n);

#endif