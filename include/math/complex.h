#ifndef MATH_COMPLEX_H_
#define MATH_COMPLEX_H_

#include <math.h>

#ifdef HIGH_PRECISION_COMPLEX

typedef struct 
{
    double re;
    double im;
} complex_t;

#else

typedef struct
{
    float re;
    float im;
} complex_t;

#endif

static inline complex_t complex_add(complex_t c1, complex_t c2)
{
    return (complex_t) 
    {
        .re = c1.re + c2.re,
        .im = c1.im + c2.im
    };
}

static inline complex_t complex_sub(complex_t c1, complex_t c2)
{
    return (complex_t) 
    {
        .re = c1.re - c2.re,
        .im = c1.im - c2.im
    };
}

static inline complex_t complex_mul(complex_t c1, complex_t c2)
{
    return (complex_t) 
    {
        .re = c1.re * c2.re - c1.im * c2.im,
        .im = c1.re * c2.im + c1.im * c2.re
    };
}

static inline complex_t complex_division(complex_t c1, complex_t c2)
{
    double denominator = c2.re * c2.re + c2.im * c2.im;
    return (complex_t) 
    {
        .re = (c1.re * c2.re + c1.im * c2.im) / denominator,
        .im = (c1.im * c2.re - c1.re * c2.im) / denominator
    };
}

static inline complex_t complex_mul_scalar(complex_t c1, double s)
{
    return (complex_t) 
    {
        .re = c1.re * s,
        .im = c1.im * s
    };
}

static inline complex_t complex_exp(complex_t c)
{   
    #ifdef HIGH_PRECISION_COMPLEX
        double ex = exp(c.re);
        return (complex_t) 
        {
            .re = ex * cos(c.im),
            .im = ex * sin(c.im)
        };
    #else
        double ex = expf(c.re);
        return (complex_t) 
        {
            .re = ex * cosf(c.im),
            .im = ex * sinf(c.im)
        };
    #endif
}

static inline void complex_add_inplace(complex_t c1, complex_t c2)
{
    c1.re += c2.re;
    c1.im += c2.im;
}

static inline void complex_sub_inplace(complex_t c1, complex_t c2)
{
    c1.re -= c2.re;
    c1.im -= c2.im;
}

static inline void complex_mul_inplace(complex_t* c1, complex_t c2)
{
    double temp_re = c1->re * c2.re - c1->im * c2.im;
    double temp_im = c1->re * c2.im + c1->im * c2.re;
    c1->re = temp_re;
    c1->im = temp_im;
}

static inline void complex_division_inplace(complex_t* c1, complex_t c2)
{
    double denominator = c2.re * c2.re + c2.im * c2.im;
    double temp_re = (c1->re * c2.re + c1->im * c2.im) / denominator;
    double temp_im = (c1->im * c2.re - c1->re * c2.im) / denominator;
    c1->re = temp_re;
    c1->im = temp_im;
}

static inline void complex_mul_scalar_inplace(complex_t c1, double s)
{
    c1.re *= s;
    c1.im *= s;
}


#endif