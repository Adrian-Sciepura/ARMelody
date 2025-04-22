#ifndef MATH_COMPLEX_NEON_H_
#define MATH_COMPLEX_NEON_H_

#include <math.h>
#include <arm_neon.h>

typedef struct complex_neon_t
{
    float32x4_t re;
    float32x4_t im;
}complex_neon_t;

static inline complex_neon_t complex_add_neon(complex_neon_t c1, complex_neon_t c2)
{
    return (complex_neon_t) 
    {
        .re = vaddq_f32(c1.re, c2.re),
        .im = vaddq_f32(c1.im, c2.im)
    };
}

static inline complex_neon_t complex_sub_neon(complex_neon_t c1, complex_neon_t c2)
{
    return (complex_neon_t) 
    {
        .re = vsubq_f32(c1.re, c2.re),
        .im = vsubq_f32(c1.im, c2.im)
    };
}

static inline complex_neon_t complex_mul_neon(complex_neon_t c1, complex_neon_t c2)
{
    return (complex_neon_t) 
    {
        .re = vaddq_f32(vmulq_f32(c1.re, c2.re), vmulq_f32(c1.im, c2.im)),
        .im = vsubq_f32(vmulq_f32(c1.re, c2.im), vmulq_f32(c1.im, c2.re))
    };
}

static inline complex_neon_t complex_division_neon(complex_neon_t c1, complex_neon_t c2)
{
    float32x4_t denominator = vaddq_f32(vmulq_f32(c2.re, c2.re), vmulq_f32(c2.im, c2.im));
    return (complex_neon_t) 
    {
        .re = vdivq_f32(vaddq_f32(vmulq_f32(c1.re, c2.re), vmulq_f32(c1.im, c2.im)), denominator),
        .im = vdivq_f32(vsubq_f32(vmulq_f32(c1.im, c2.re), vmulq_f32(c1.re, c2.im)), denominator)
    };
}

static inline float32x4_t wrap_to_pi(float32x4_t x)
{
    float32x4_t k = vmulq_f32(x, vdupq_n_f32(1 / (2 * M_PI)));
    k = vcvtq_f32_s32(vcvtq_s32_f32(k)); // floor(x / 2π)
    return vsubq_f32(x, vmulq_f32(k, vdupq_n_f32(2 * M_PI)));
}


// if it takies too long, we can use lower-order polynomials like x5 max
static inline float32x4_t vsinq_f32(float32x4_t x)
{
    x = wrap_to_pi(x);
    float32x4_t result = vdupq_n_f32(0);
    float32x4_t x2 = vmulq_f32(x, x);
    float32x4_t x3 = vmulq_f32(x2, x);
    float32x4_t x5 = vmulq_f32(x3, x2);
    float32x4_t x7 = vmulq_f32(x5, x2);
    result = vfmsq_f32(result, x3, vdupq_n_f32(1.0f / 6.0f));
    result = vfmaq_f32(result, x5, vdupq_n_f32(1.0f / 120.0f));
    result = vfmsq_f32(result, x7, vdupq_n_f32(1.0f / 5040.0f));
    return result;
}
static inline float32x4_t vcosq_f32(float32x4_t x)
{
    x = wrap_to_pi(x);
    float32x4_t x2 = vmulq_f32(x, x);
    float32x4_t x4 = vmulq_f32(x2, x2);
    float32x4_t x6 = vmulq_f32(x4, x2);

    float32x4_t result = vdupq_n_f32(1.0f);
    result = vfmsq_f32(result, x2, vdupq_n_f32(1.0f / 2.0f));
    result = vfmaq_f32(result, x4, vdupq_n_f32(1.0f / 24.0f));
    result = vfmsq_f32(result, x6, vdupq_n_f32(1.0f / 720.0f));
    return result;
}

static inline float32x4_t vexpq_f32(float32x4_t x)
{
    // Optional: clip to prevent overflow,
    //x = vmaxq_f32(vminq_f32(x, vdupq_n_f32(10.0f)), vdupq_n_f32(-10.0f));

    float32x4_t result = vdupq_n_f32(1.0f);
    float32x4_t term = vdupq_n_f32(1.0f);

    static const float inv_fact[9] = {
        1.0f / 1.0f, 1.0f / 2.0f, 1.0f / 3.0f, 1.0f / 4.0f,
        1.0f / 5.0f, 1.0f / 6.0f, 1.0f / 7.0f, 1.0f / 8.0f,
        1.0f / 9.0f
    };

    for (int i = 0; i < 9; i++) {
        term = vmulq_f32(term, x);
        result = vfmaq_f32(result, term, vdupq_n_f32(inv_fact[i]));
    }

    return result;
}

static inline complex_neon_t complex_mul_scalar_neon(complex_neon_t c1, float s)
{
    float32x4_t scalar = vdupq_n_f32(s);
    return (complex_neon_t) 
    {
        .re = vmulq_f32(c1.re, scalar),
        .im = vmulq_f32(c1.im, scalar)
    };
}
static inline complex_neon_t complex_exp_neon(complex_neon_t c)
{   
    float32x4_t ex = vexpq_f32(c.re);
    float32x4_t cos_val = vcosq_f32(c.im);
    float32x4_t sin_val = vsinq_f32(c.im);
    return (complex_neon_t) 
    {
        .re = vmulq_f32(ex, cos_val),
        .im = vmulq_f32(ex, sin_val)
    };
}




#endif