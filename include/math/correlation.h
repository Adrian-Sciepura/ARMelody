#ifndef MATH_CORR_H_
#define MATH_CORR_H_

#include <math/complex.h>
#include <math/complex_neon.h>

typedef void(*correlation_callback)(complex_t* orginal_data, complex_t* potential_match_data, int n);

void correlation(complex_t* orginal_data, complex_t* potential_match_data, int n);

void correlation_interpretation(complex_t* data, int n);

typedef void(*correlation_callback_neon)(complex_neon_t* orginal_data, complex_neon_t* potential_match_data, int n);

void correlation_neon(complex_t* orginal_data, complex_t* potential_match_data, int n);

void correlation_interpretation_neon(complex_neon_t* data, int n);

#endif