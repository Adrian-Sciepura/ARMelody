#include <math/fft.h>
#include <math/consts.h>
#include <stdlib.h>
#include <stdio.h>

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

    for(int len = 2; len <= n; len <<= 1)
    {
        double angle = 2 * PI / len * (invert ? -1 : 1);
        complex_t wlen = { .re = cos(angle), .im = sin(angle) };
        for(int i = 0; i < n; i += len)
        {
            complex_t w = { .re = 1, .im = 0 };
            for(int j = 0; j < len / 2; j++)
            {
                complex_t u = data[i + j];
                complex_t v = complex_mul(w, data[i + j + len / 2]);
                data[i + j] = complex_add(u, v);
                data[i + j + len / 2] = complex_sub(u, v);
                w = complex_mul(w, wlen);
            }
        }
    }

    if(invert)
    {
        for(int i = 0; i < n; i++)
        {
            data[i].re /= n;
            data[i].im /= n;
        }
    }
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