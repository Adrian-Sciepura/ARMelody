#ifndef MATH_CORR_H_
#define MATH_CORR_H_

#include <math/complex.h>

typedef void(*correlation_callback)(complex_t* orginal_data, complex_t* potential_match_data, int n);

void correlation(complex_t* orginal_data, complex_t* potential_match_data, int n);

void correlation_interpretation(complex_t* data, int n);

#endif