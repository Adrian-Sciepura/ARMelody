#include <math/fft.h>
#include <math/consts.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>

void cooley_tukey_fft(complex_t* data, int n)
{
    if(n <= 1)
    {
        return;
    }

    complex_t* split = (complex_t*)malloc(sizeof(complex_t)*n);
    complex_t* odd = split;
    complex_t* even = &split[n/2];

    for(int i = 0; i < n/2; i++)
    {
        even[i] = data[i*2];
        odd[i] = data[i*2+1];
    }

    cooley_tukey_fft(odd, n/2);
    cooley_tukey_fft(even, n/2);

    for(int k = 0; k < n/2; k++)
    {
        complex_t tmp = complex_exp((complex_t){ .re = 0, .im = -2 * PI  * k / n });
        complex_mul_inplace(&tmp, odd[k]);
        data[k] = complex_add(even[k], tmp);
        data[n/2 + k] = complex_sub(even[k], tmp);
    }

    free(split);
}

void fft_iterative(complex_t* data, int n, bool invert)
{
    for(int i = 1, j = 0; i < n; i++)
    {
        int bit = n >> 1;
        for(; j & bit; bit >>= 1)
            j ^= bit;
        
        j ^= bit;
        if(i < j)
        {
            complex_t tmp = data[i];
            data[i] = data[j];
            data[j] = tmp;
        }
    }

    // for(int i = 0; i < n; i++)
    //     printf("data[%d] = %f + %fi\n", i, data[i].re, data[i].im);

    // printf("\n");

    clock_t start = clock();

    for(int len = 4; len <= n; len <<= 1)
    {
        //printf("===========================> len: %d\n", len);
        double angle = 2 * PI / len * (invert ? -1 : 1);
        //printf("==========> angle: %f\n", angle);
        complex_t wlen = { .re = cos(angle), .im = sin(angle) };
        //printf("==========> wlen: %f + %fi\n", wlen.re, wlen.im);
        for(int i = 0; i < n; i += len)
        {
            complex_t w = { .re = 1, .im = 0 };
            //printf("==========> w: %f + %fi\n", w.re, w.im);
            for(int j = 0; j < len / 2; j++)
            {
                complex_t u = data[i + j];
                complex_t v = complex_mul(w, data[i + j + len / 2]);
                
                //printf("\t\tu: %f + %fi\n", u.re, u.im);
                //printf("\t\tvw: %f + %fi\n", v.re, v.im);

                data[i + j] = complex_add(u, v);
                data[i + j + len / 2] = complex_sub(u, v);
                w = complex_mul(w, wlen);
                //printf("\t\tw: %f + %fi\n", w.re, w.im);
            }

            //printf("====================\n");
        }
    }

    printf("Time: %fs\n", (double)(clock() - start) / CLOCKS_PER_SEC);

    if(invert)
    {
        for(int i = 0; i < n; i++)
        {
            data[i].re /= n;
            data[i].im /= n;
        }
    }
}

// Performs multiplication of two complex numbers
// ----- Data -----
// num1 = a + bi
// num2 = c + di
// num3 = e + fi
// num4 = g + hi
// ----- Result -----
// res = [num1 * num2, num3 * num4]
static inline float32x4_t multiply_two_complex(float32x4_t abef, float32x4_t ccgg, float32x4_t ddhh)
{
    static uint32x4_t mask_elements_0_and_2 = { 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000 };
    static uint32x4_t mask_elements_1_and_3 = { 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF };
 
    float32x4_t temp1, temp2, temp3;

    // ----- Stage 1 -----

    // temp1 = [ac, bc, eg, fg]
    temp1 = vmulq_f32(abef, ccgg);
    
    // temp2 = [b, a, f, e]
    temp2 = vrev64q_f32(abef);
    
    // temp2 = [bd, ad, fh, eh]
    temp2 = vmulq_f32(temp2, ddhh);

    // ----- Stage 2 -----
    
    // temp3 = [ac, bc, eg, fg]
    temp3 = temp1;
    
    // temp1 = [ac - bd, bc - ad, eg - fh, fg - eh]
    temp1 = vsubq_f32(temp1, temp2);
    
    // temp2 = [ac + bd, bc + ad, eg + fh, fg + eh]
    temp2 = vaddq_f32(temp3, temp2);

    // ----- Stage 3 -----

    // temp3 = [0, 0, 0, 0]
    temp3 = vdupq_n_f32(0);

    // result = [ac - bd, bc + ad, eg - fh, fg + eh]
    return vaddq_f32(
        vbslq_f32(mask_elements_0_and_2, temp1, temp3),
        vbslq_f32(mask_elements_1_and_3, temp2, temp3)
    );
}


void fft_iterative_neon_fixed(complex_t* data, int n, bool invert)
{
    for(int i = 1, j = 0; i < n; i++)
    {
        int bit = n >> 1;
        for(; j & bit; bit >>= 1)
            j ^= bit;
        
        j ^= bit;
        if(i < j)
        {
            complex_t tmp = data[i];
            data[i] = data[j];
            data[j] = tmp;
        }
    }

    uint64_t* data_float_ptr = (uint64_t*)data;
    clock_t start = clock();
    static uint8x16_t idx =  {0, 1, 2, 3, 0, 1, 2, 3, 8, 9, 10, 11, 8, 9, 10, 11};
    static uint8x16_t idx2 = {4, 5, 6, 7, 4, 5, 6, 7, 12, 13, 14, 15, 12, 13, 14, 15};
    

    for(int len = 4; len <= n; len <<= 1)
    {
        double angle = 2 * PI / len * (invert ? -1 : 1);
        float angle_sin = sin(angle);
        float angle_cos = cos(angle);

        float32x4_t wlen = { angle_cos, angle_sin, angle_cos, angle_sin };
        float32x4_t wlen_re = { angle_cos, angle_cos, angle_cos, angle_cos };
        float32x4_t wlen_im = { angle_sin, angle_sin, angle_sin, angle_sin };

        //printf("===========================> len: %d\n", len);
        //printf("==========> angle: %f\n", angle);
        //printf("==========> wlen: %f + %fi, %f + %fi\n", vgetq_lane_f32(wlen, 0), vgetq_lane_f32(wlen, 1), vgetq_lane_f32(wlen, 2), vgetq_lane_f32(wlen, 3));

        for(int i = 0; i < n; i += len)
        {    
            float32x4_t w_re = { 1, 1, angle_cos, angle_cos };
            float32x4_t w_im = { 0, 0, angle_sin, angle_sin };

            //printf("==========> w: %f + %fi, %f + %fi\n", w_re[0], w_im[0], w_re[2], w_im[2]);

            for(int j = 0; j < len / 2; j += 2)
            {

                float32x4_t u = vld1q_f32(&data_float_ptr[i + j]);
                float32x4_t v = vld1q_f32(&data_float_ptr[i + j + (len / 2)]);

                v = multiply_two_complex(v, w_re, w_im);
                //printf("\t\tu: %f + %fi, %f + %fi\n", vgetq_lane_f32(u, 0), vgetq_lane_f32(u, 1), vgetq_lane_f32(u, 2), vgetq_lane_f32(u, 3));
                //printf("\t\tvw: %f + %fi, %f + %fi\n", vgetq_lane_f32(v, 0), vgetq_lane_f32(v, 1), vgetq_lane_f32(v, 2), vgetq_lane_f32(v, 3));

                vst1q_f32((float*)&data[i + j], vaddq_f32(u, v));
                vst1q_f32((float*)&data[i + j + (len / 2)], vsubq_f32(u, v));

                uint8x16_t temp = vreinterpretq_u8_f32(multiply_two_complex(multiply_two_complex(wlen, w_re, w_im), wlen_re, wlen_im));
                //vst1q_f32(buffer, multiply_two_complex(multiply_two_complex(wlen, w_re, w_im), wlen_re, wlen_im));
                
                w_re = vreinterpretq_f32_u8(vqtbl1q_u8(temp, idx));
                w_im = vreinterpretq_f32_u8(vqtbl1q_u8(temp, idx2));

                //printf("\t\tw: %f + %fi, %f + %fi\n", w_re[0], w_im[0], w_re[2], w_im[2]);
            }

            //printf("====================\n");
        }
    }
    printf("Time: %fs\n", (double)(clock() - start) / CLOCKS_PER_SEC);
}



void fft_iterative_neon(complex_t* data, int n, bool invert)
{
    for(int i = 1, j = 0; i < n; i++)
    {
        int bit = n >> 1;
        for(; j & bit; bit >>= 1)
            j ^= bit;
        
        j ^= bit;
        if(i < j)
        {
            complex_t tmp = data[i];
            data[i] = data[j];
            data[j] = tmp;
        }
    }


    float *data_real = (float*)aligned_alloc(16, sizeof(float) * (n+3));
    float *data_imag = (float*)aligned_alloc(16, sizeof(float) * (n+3));

/*    if (posix_memalign((void**)&data_real, 16, sizeof(float) * n) != 0 ||
        posix_memalign((void**)&data_imag, 16, sizeof(float) * n) != 0) {
        printf("Memory alignment failed\n");
        exit(1);
    }*/
    for(int i = 0; i < n; i++)
    {
        data_real[i] = data[i].re;
        data_imag[i] = data[i].im;
    } 

    for(int len = 2; len <= n; len <<= 1)
    {
        float angle = 2 * PI / len * (invert ? -1 : 1);
        float wlen_re = cos(angle);
        float wlen_im = sin(angle);
        for(int i = 0; i < n; i += len)
        {
            float32x4_t w_re = vdupq_n_f32(1.0f);
            float32x4_t w_im = vdupq_n_f32(0.0f);
            float32x4_t wlen_re_vec = vdupq_n_f32(wlen_re);
            float32x4_t wlen_im_vec = vdupq_n_f32(wlen_im);

            for(int j = 0; j < len / 2; j += 4)
            {
                float32x4_t u_re = vld1q_f32(&data_real[i + j]);
                float32x4_t u_im = vld1q_f32(&data_imag[i + j]);

                // Load v (data[i + j + len / 2]) and compute w * v
                float32x4_t v_re = vld1q_f32(&data_real[i + j + len / 2]);
                float32x4_t v_im = vld1q_f32(&data_imag[i + j + len / 2]);
                float32x4_t temp_re = vsubq_f32(vmulq_f32(w_re, v_re), vmulq_f32(w_im, v_im));
                float32x4_t temp_im = vaddq_f32(vmulq_f32(w_re, v_im), vmulq_f32(w_im, v_re));

                // // Update data[i + j] = u + w * v
                vst1q_f32(&data_real[i + j], vaddq_f32(u_re, temp_re));
                vst1q_f32(&data_imag[i + j], vaddq_f32(u_im, temp_im));

                // Update data[i + j + len / 2] = u - w * v
                // printf("i: %d, j: %d, len: %d, index: %d, n: %d\n", i, j, len, i + j + len / 2, n);
                // if (((uintptr_t)&data_real[i + j + len / 2] % 16) != 0) {
                //     printf("Unaligned memory address: %p\n", &data_real[i + j + len / 2]);
                //     exit(1);
                // }
                
                // if (((uintptr_t)&data_imag[i + j + len / 2] % 16) != 0) {
                //     printf("Unaligned memory address: %p\n", &data_imag[i + j + len / 2]);
                //     exit(1);
                // }
                
                vst1q_f32(&data_real[i + j + len / 2], vsubq_f32(u_re, temp_re));
                //vst1q_f32(&data_imag[i + j + len / 2], vsubq_f32(u_im, temp_im));

                // // Update w = w * wlen
                float32x4_t new_w_re = vsubq_f32(vmulq_f32(w_re, wlen_re_vec), vmulq_f32(w_im, wlen_im_vec));
                float32x4_t new_w_im = vaddq_f32(vmulq_f32(w_re, wlen_im_vec), vmulq_f32(w_im, wlen_re_vec));
                w_re = new_w_re;
                w_im = new_w_im;
            }
        }
    }
    
    if(invert)
    {
        float32x4_t n_vec = vdupq_n_f32(n);
        for(int i = 0; i < n; i += 4)
        {
            float32x4_t re = vld1q_f32(&data[i].re);
            float32x4_t im = vld1q_f32(&data[i].im);
            vst1q_f32(&data[i].re, vdivq_f32(re, n_vec));
            vst1q_f32(&data[i].im, vdivq_f32(im, n_vec));
        }
    }
    
    for(int i = 0; i < n; i++)
    {
        data[i].re = data_real[i];
        data[i].im = data_imag[i];
    }
    
    free(data_real);
    free(data_imag);
    
}

void cooley_tukey_ifft(complex_t* data, int n)
{
    for(int i = 0; i < n; i++)
    {
        data[i].im = -data[i].im;
    }
    cooley_tukey_fft(data, n);
    for(int i = 0; i < n; i++)
    {
        data[i].re = data[i].re / n;
        data[i].im = data[i].im / -n;
    }
}