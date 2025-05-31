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
        .re = vsubq_f32(vmulq_f32(c1.re, c2.re), vmulq_f32(c1.im, c2.im)),
        .im = vaddq_f32(vmulq_f32(c1.re, c2.im), vmulq_f32(c1.im, c2.re))
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
    const float32x4_t two_pi = vdupq_n_f32(2.0f * M_PI);
    const float32x4_t inv_two_pi = vdupq_n_f32(1.0f / (2.0f * M_PI));
    float32x4_t k = vmulq_f32(x, inv_two_pi);
    k = vrndnq_f32(k);
    return vsubq_f32(x, vmulq_f32(k, two_pi));
}

static inline float32x4_t vsinq_f32(float32x4_t x)
{
    x = wrap_to_pi(x);

    // Use symmetries: sin(x) = -sin(-x)
    uint32x4_t sign_mask = vcltq_f32(x, vdupq_n_f32(0.0f)); // x < 0
    float32x4_t x_abs = vabsq_f32(x); // work with positive x

    const float32x4_t pi = vdupq_n_f32(M_PI);
    const float32x4_t half_pi = vdupq_n_f32(M_PI * 0.5f);
    uint32x4_t greater_half_pi = vcgtq_f32(x_abs, half_pi); // x > pi/2
    x_abs = vbslq_f32(greater_half_pi, vsubq_f32(pi, x_abs), x_abs);

    // Polynomial approximation: sin(x) ≈ x * (1 + x^2 * (p1 + x^2 * (p2 + x^2 * p3)))
    float32x4_t x2 = vmulq_f32(x_abs, x_abs);

    const float32x4_t p3 = vdupq_n_f32(-0.0001950727f);
    const float32x4_t p2 = vdupq_n_f32( 0.0083320758f);
    const float32x4_t p1 = vdupq_n_f32(-0.1666665247f);

    float32x4_t poly = vfmaq_f32(p2, p3, x2);    // p2 + p3 * x^2
    poly = vfmaq_f32(p1, poly, x2);               // p1 + (p2 + p3 * x^2) * x^2
    poly = vmulq_f32(poly, x2);                   // (p1 + p2*x^2 + p3*x^4) * x^2

    float32x4_t result = vfmaq_f32(x_abs, poly, x_abs);   // x + x * poly

    result = vbslq_f32(sign_mask, vnegq_f32(result), result); // apply sign

    return result;
}

static inline float32x4_t vcosq_f32(float32x4_t x)
{
    x = wrap_to_pi(x);

    float32x4_t x_abs = vabsq_f32(x); // cos(-x) = cos(x), so always positive x

    const float32x4_t pi = vdupq_n_f32(M_PI);
    const float32x4_t half_pi = vdupq_n_f32(M_PI * 0.5f);
    
    // If x > pi/2, map: cos(x) = -cos(pi - x)
    uint32x4_t greater_half_pi = vcgtq_f32(x_abs, half_pi);
    float32x4_t x_mapped = vbslq_f32(greater_half_pi, vsubq_f32(pi, x_abs), x_abs);

    float32x4_t x2 = vmulq_f32(x_mapped, x_mapped);

    // Minimax coefficients for cos(x) ~ 1 + x^2*(p1 + x^2*(p2 + x^2*p3))
    const float32x4_t p3 = vdupq_n_f32( 0.0002605556f);
    const float32x4_t p2 = vdupq_n_f32(-0.0013888889f);
    const float32x4_t p1 = vdupq_n_f32( 0.0416666664f);
    const float32x4_t p0 = vdupq_n_f32(-0.5f);            // -1/2

    float32x4_t poly = vfmaq_f32(p2, p3, x2);     // p2 + p3 * x^2
    poly = vfmaq_f32(p1, poly, x2);               // p1 + (p2 + p3 * x^2) * x^2
    poly = vfmaq_f32(p0, poly, x2);               // p0 + (p1 + p2*x^2 + p3*x^4) * x^2
    poly = vmulq_f32(poly, x2);                   // * x^2
    

    float32x4_t result = vaddq_f32(vdupq_n_f32(1.0f), poly); // 1 + poly

    result = vbslq_f32(greater_half_pi, vnegq_f32(result), result); // apply sign
    
    // OPTIONAL MAYBE SLOW MORE ACCURATE Special case: very close to pi/2, force cos(x) ≈ 0
    uint32x4_t near_half_pi = vcltq_f32(vabsq_f32(vsubq_f32(x_abs, half_pi)), vdupq_n_f32(0.01f));
    result = vbslq_f32(near_half_pi, vdupq_n_f32(0.0f), result);

    return result;
}

static inline void vsincosq_f32(float32x4_t x, float32x4_t* sin_out, float32x4_t* cos_out)
{
    x = wrap_to_pi(x);

    // sin(-x) = -sin(x), cos(-x) = cos(x)
    uint32x4_t sign_mask = vcltq_f32(x, vdupq_n_f32(0.0f));
    float32x4_t x_abs = vabsq_f32(x);

    const float32x4_t pi = vdupq_n_f32(M_PI);
    const float32x4_t half_pi = vdupq_n_f32(M_PI * 0.5f);

    uint32x4_t greater_half_pi = vcgtq_f32(x_abs, half_pi);
    float32x4_t x_mapped = vbslq_f32(greater_half_pi, vsubq_f32(pi, x_abs), x_abs);

    float32x4_t x2 = vmulq_f32(x_mapped, x_mapped);

    // --- Sine polynomial: x + x^3*p1 + x^5*p2 + x^7*p3 ---
    const float32x4_t sin_p3 = vdupq_n_f32(-0.0001950727f);
    const float32x4_t sin_p2 = vdupq_n_f32( 0.0083320758f);
    const float32x4_t sin_p1 = vdupq_n_f32(-0.1666665247f);

    float32x4_t sin_poly = vfmaq_f32(sin_p2, sin_p3, x2);    // sin_p2 + sin_p3*x^2
    sin_poly = vfmaq_f32(sin_p1, sin_poly, x2);              // sin_p1 + ...
    sin_poly = vmulq_f32(sin_poly, x2);                     // * x^2

    float32x4_t sin_result = vfmaq_f32(x_mapped, sin_poly, x_mapped); // x + x*poly
    sin_result = vbslq_f32(sign_mask, vnegq_f32(sin_result), sin_result);
    
    // --- Cosine polynomial: 1 + x^2*(p0 + x^2*(p1 + x^2*(p2 + x^2*p3))) ---
    const float32x4_t cos_p3 = vdupq_n_f32( 0.0000248016f);
    const float32x4_t cos_p2 = vdupq_n_f32(-0.0013888889f);
    const float32x4_t cos_p1 = vdupq_n_f32( 0.0416666664f);
    const float32x4_t cos_p0 = vdupq_n_f32(-0.5f);

    float32x4_t cos_poly = vfmaq_f32(cos_p2, cos_p3, x2);    // cos_p2 + cos_p3*x^2
    cos_poly = vfmaq_f32(cos_p1, cos_poly, x2);              // cos_p1 + ...
    cos_poly = vfmaq_f32(cos_p0, cos_poly, x2);              // cos_p0 + ...
    cos_poly = vmulq_f32(cos_poly, x2);                     // * x^2

    float32x4_t cos_result = vaddq_f32(vdupq_n_f32(1.0f), cos_poly); // 1 + poly
    cos_result = vbslq_f32(greater_half_pi, vnegq_f32(cos_result), cos_result);

    // Output
    *sin_out = sin_result;
    *cos_out = cos_result;
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

static inline complex_neon_t complex_mul_scalar_neon(complex_neon_t c1, float32x4_t s)
{
    return (complex_neon_t) 
    {
        .re = vmulq_f32(c1.re, s),
        .im = vmulq_f32(c1.im, s)
    };
}

static inline complex_neon_t complex_exp_neon(complex_neon_t c)
{   
    float32x4_t ex = vexpq_f32(c.re);
    float32x4_t cos_val, sin_val;
    vsincosq_f32(c.im, &sin_val, &cos_val);
    return (complex_neon_t) 
    {
        .re = vmulq_f32(ex, cos_val),
        .im = vmulq_f32(ex, sin_val)
    };
}

#endif