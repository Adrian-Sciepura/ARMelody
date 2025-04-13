#include <math/fft.h>
#include <math/consts.h>
#include <stdlib.h>

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
        complex_mul_inplace(tmp, odd[k]);
        data[k] = complex_add(even[k], tmp);
        data[n/2 + k] = complex_sub(even[k], tmp);
    }

    free(split);
}

void cooley_turkey_ifft(complex_t* data, int n)
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