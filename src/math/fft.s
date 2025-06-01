.global fft_iterative_neon
.extern __sincos

//      ┌────────────────────┐                                                       
//      │                    │                                                       
//      │ fft_iterative_neon ├──────────────────────────────────────────────────────┐
//      │                    │                                                      │
//      └────┬───────────────┘                                                      │
//           │                                                                      │
//      ┌────▼───────────────────────────────┐   ┌──────────────────────────────┐   │
//      │                                    │   │                              │   │
// ┌────►  bit_reversal_inner_loop_condition ├───► bit_reversal_inner_loop_done │   │
// │    │                                    │   │                              │   │
// │    └────┬───────────────────────────────┘   └──────────────┬───────────────┘   │
// │         │                                                  │                   │
// │    ┌────▼─────────────────────────┐                        │                   │
// │    │                              │                        │                   │
// │    │ bit_reversal_inner_loop_body ◄───┐                    │                   │
// │    │                              │   │                    │                   │
// │    └────┬─────────────────────┬───┘   │                    │                   │
// │         │                     │       │                    │                   │
// │    ┌────▼──────────────┐      └───────┘                    │                   │
// │    │                   │                                   │                   │
// │    │ bit_reversal_swap │◄──────────────────────────────────┘                   │
// │    │                   │                                                       │
// │    └────┬──────────────┘                                                       │
// │         │                                                                      │
// │    ┌────▼──────────────────────────────┐                                       │
// │    │                                   │                                       │
// └────┼ bit_reversal_outer_loop_increment │                                       │
//      │                                   │                                       │
//      └────┬──────────────────────────────┘                                       │
//           │                                                                      │
//      ┌────▼────────────────┐                                                     │
//      │                     │                                                     │
//      │ fft_outer_loop_body │                                                     │
//      │                     │                                                     │
//      └────┬────────────────┘                                                     │
//           │                                                                      │
//      ┌────▼──────────────────┐                                                   │
//      │                       │                                                   │
//      │ fft_outer_loop_body_2 │◄─────────────────────┐                            │
//      │                       │                      │                            │
//      └────┬──────────────────┘                      │                            │
//           │                                         │                            │
//      ┌────▼─────────────────┐                       │                            │
//      │                      │                       │                            │
//      │ fft_middle_loop_body │◄─────────────┐        │                            │
//      │                      │              │        │                            │
//      └────┬─────────────────┘              │        │                            │
//           │                                │        │                            │
//      ┌────▼────────────────┐               │        │                            │
//      │                     │◄─────┐        │        │                            │
//      │ fft_inner_loop_body │      │        │        │                            │
//      │                     ├──────┘        │        │                            │
//      └────┬────────────────┘               │        │                            │
//           │                                │        │                            │
//      ┌────▼──────────────────────┐         │        │                            │
//      │                           │         │        │                            │
//      │ fft_middle_loop_condition ├─────────┘        │                            │
//      │                           │                  │                            │
//      └────┬──────────────────────┘                  │                            │
//           │                                         │                            │
//      ┌────▼─────────────────────┐                   │                            │
//      │                          │                   │                            │
//      │ fft_outer_loop_condition ├───────────────────┘                            │
//      │                          │                                                │
//      └────┬─────────────────────┘                                                │
//           │                                                                      │
//      ┌────▼───────────────┐     ┌─────────────┐     ┌─────────────────────┐      │
//      │                    │     │             │     │                     │      │
//      │ fft_outer_loop_end ├────►│ skip_invert │     │ not_enough_elements │◄─────┘
//      │                    │     │             │     │                     │       
//      └────┬───────────────┘     └───┬─────────┘     └──────┬───────┬──────┘       
//           │                         │                      │       │              
//      ┌────▼───┐                     │                      │       │              
//      │        │                     │                      │       │              
//      │ invert │◄────────────────────┼──────────────────────┘       │              
//      │        │                     │                              │              
//      └────┬───┘                     │                              │              
//           │                         │                              │              
//      ┌────▼────────┐                │                              │              
//      │             │◄─────┐         │                              │              
//      │ invert_loop │      │         │                              │              
//      │             ├──────┘         │                              │              
//      └────┬────────┘                │                              │              
//           │                         │                              │              
//      ┌────▼───────────────────┐     │                              │              
//      │                        │     │                              │              
//      │ fft_iterative_neon_end │◄────┴──────────────────────────────┘              
//      │                        │                                                   
//      └────────────────────────┘                                                                                

fft_iterative_neon:
//
//  ─────── IN ────────
//
//  x0 - data
//  x1 - n
//  x2 - invert
//
//  ─────── OUT ───────
//
//  [SP + 0] = x29
//  [SP + 8] = x30
//  [SP + 16] = x19
//  [SP + 24] = x20
//  [SP + 32] = x21
//  [SP + 40] = x22
//  [SP + 48] = x23
//  [SP + 56] = x24
//  [SP + 64] = x25
//  [SP + 72] = ????
//  [SP + 80] = d8
//  [SP + 88] = d9
//  [SP + 96] = d10
//  [SP + 104] = d11
//  [SP + 112] = ????
//  [SP + 120] = ????
//
//  w1 - j
//  x2 - data + i * 8
//  w3 - bit
//  w20 - i
//  x21 - data
//  w22 - n
//  w25 - invert
//
//  ───────────────────
//
    stp x29, x30, [sp, #-128]!              // sp ─= 128
    mov x29, sp
    stp x21, x22, [sp, #32]
    str x25, [sp, #64]
    mov x21, x0                             // x21 = data
    mov w22, w1                             // w22 = n
    and w25, w2, #0xff                      // w25 = invert & 0xff
    cmp w1, #0x1
    b.le not_enough_elements                // if n <= 1
    stp x19, x20, [sp, #16]
    stp x23, x24, [sp, #48]
    stp d8, d9, [sp, #80]
    stp d10, d11, [sp, #96]
    asr w3, w1, #1                          // bit = n >> 1
    add x2, x0, #0x8                        // data += 8
    mov w1, #0x0                            // j = 0
    mov w20, #0x1                           // i = 1
    b bit_reversal_inner_loop_condition
bit_reversal_inner_loop_done:
//
//  ─────── IN ────────
//
//  w3 - bit
//
//  ─────── OUT ───────
//
//  w0 - bit
//
//  ───────────────────
//
    mov w0, w3                              // w0 = bit
    b bit_reversal_swap
bit_reversal_outer_loop_increment:
//
//  ─────── IN ────────
//
//  x2 - data + i * 8
//  w20 - i
//  w22 - n
//
//  ─────── OUT ───────
//
//  x2 - data + (i + 1) * 8
//  w20 - i + 1
//
//  ───────────────────
//
    add w20, w20, #0x1                      // i += 1
    add x2, x2, #0x8                        // data += 8
    cmp w22, w20
    b.eq fft_outer_loop_body                // if j == n
bit_reversal_inner_loop_condition:
//
//  ─────── IN ────────
//
//  w1 - j
//  w3 - bit
//
//  ─────── OUT ───────
//
//  w0 - bit
//
//  ───────────────────
//
    tst w3, w1
    b.eq bit_reversal_inner_loop_done       // if j == bit 
    mov w0, w3                              // w0 = bit
bit_reversal_inner_loop_body:
//
//  ─────── IN ────────
//
//  w0 - bit
//  w1 - j
//
//  ─────── OUT ───────
//
//  w0 - bit / 2
//  w1 - j ^ bit
//
//  ───────────────────
//
    eor w1, w1, w0                          // j = j ^ bit
    asr w0, w0, #1                          // bit = bit >> 1
    tst w1, w0
    b.ne bit_reversal_inner_loop_body       // if j != bit
bit_reversal_swap:
//
//  ─────── IN ────────
//
//  w0 - bit
//  w1 - j
//  x2 - data + i * 8
//  w20 - i
//  x21 - data
//
//  ─────── OUT ───────
//
//  w1 - j ^ bit
//  x4 - data + j * 8
//
//  ───────────────────
//
    eor w1, w1, w0                          // j = j ^ bit
    cmp w1, w20
    b.le bit_reversal_outer_loop_increment  // if i < j
    ldr s1, [x2]                            // s1 = data[i].re
    ldr s0, [x2, #4]                        // s0 = data[i].im
    sbfiz x0, x1, #3, #32                   // bit = (int32_t)(j & 0xFFFFFFFF) << 3 = j * 8
    add x4, x21, x0                         // x4 = data + bit
    ldr x5, [x21, w1, sxtw#3]               // x5 = *(data + j * 8) = data[j] 
    str x5, [x2]                            // data[i] = data[j]
    str s1, [x4]                            // data[j].re = data[i].re
    str s0, [x4, #4]                        // data[j].im = data[i].im
    b bit_reversal_outer_loop_increment
fft_outer_loop_body:
//
//  ─────── IN ────────
//
//  w25 - invert
//
//  ─────── OUT ───────
//
//  w0 - invert
//  w19 - 2
//  x23 - SP + 112
//  x24 - SP + 120
//
//  d0 - 1
//  s8 - w.re
//  v9 - int32_t [?, ?, 0, 0]
//  d10 - invert val
//  d11 - 2 * PI
//
//  ───────────────────
//
    sxtb w0, w25                            // w0 = (int8_t)(w25 & 0xFF);
    cmp w0, #0x0                            // if w0 == 0
    fmov d10, #-1.000000000000000000e+00    // d10 = ─1
    fmov d0, #1.000000000000000000e+00      // d0 = 1
    fcsel d10, d10, d0, ne                  // d10 = w0 != 0 ? d10 : d0 
    mov w19, #0x2
    add x24, sp, #120
    add x23, sp, #112
    ldr x0, =0x401921fb54442d18             // x0 = 2 * PI (in double IEEE 754 format)
    fmov d11, x0                            
    movi v9.2s, #0x0                        // v9[0] = 0, v9[1] = 0
    fmov s8, #1.000000000000000000e+00      // w.re = 1
    b fft_outer_loop_body_2
fft_middle_loop_body:
//
//  ─────── IN ────────
//
//  x6 - data
// 
//  s8 - w.re
//  s9 - w.im
//
//  ─────── OUT ───────
//
//  x0 - data
//  w1 - j
//
//  s1 - w.re
//  s2 - w.im
//
//  ───────────────────
//
    mov x0, x6                              // x0 = data

    mov v1.s[0], v8.s[0]
    mov v1.s[1], v9.s[0]

    mov v2.s[0], v16.s[0]
    mov v2.s[1], v7.s[0]

    mov w1, #0x0                            // j = 0
fft_inner_loop_body:
//
//  ─────── IN ────────
//
//  x0 - data
//  x2 - len * 4
//  x3 - len * 4 + 4
//  
//  s1 - w.re
//  s2 - w.im
//  s7 - wlen.im
//  s16 - wlen.re
//
//  ─────── OUT ───────
//
//  x0 - data + (len / 2) * 8
//  w1 - len / 2 - 1
//
//  ───────────────────
//
    add x3, x0, x2
    ld1 {v3.2s}, [x0]                       // v3 = ?, ?, data[i + j].re, data[i + j].im 
    ld1 {v4.2s}, [x3]                       // v4 = ?, ?, data[i + j + len/2].re, data[i + j + len/2].im

    movi v5.2s, #0
    fcmla v5.2s, v4.2s, v1.2s, #0
    fcmla v5.2s, v4.2s, v1.2s, #90          // v5 = v

    fadd v6.2s, v3.2s, v5.2s                // v6 = u + v
    st1 {v6.2s}, [x0]                       // data[i + j] = u + v

    fsub v6.2s, v3.2s, v5.2s                // v6 = u - v
    st1 {v6.2s}, [x3]                       // data[i + j + len/2] = u - v

    movi v5.2s, #0
    fcmla v5.2s, v1.2s, v2.2s, #0
    fcmla v5.2s, v1.2s, v2.2s, #90          // v5 = w * wlen
    mov v1.16b, v5.16b

    add w1, w1, #0x1                        // j += 1
    add x0, x0, #0x8                        // data = data + 8
    cmp w1, w4                              // if j < len / 2
    b.lt fft_inner_loop_body
fft_middle_loop_condition:
//
//  ─────── IN ────────
//
//  w5 - i
//  x6 - data
//  x7 - len * 8
//  w19 - len
//  w20 - n
//
//  ─────── OUT ───────
//
//  ───────────────────
//
    add w5, w5, w19                         // i += len
    add x6, x6, x7                          // data += len * 8
    cmp w5, w20                             
    b.ge fft_outer_loop_condition           // if i >= n
    b fft_middle_loop_body
fft_outer_loop_condition:
//
//  ─────── IN ────────
//
//  w19 - len
//  w20 - n
//
//  ─────── OUT ───────
//
//  ───────────────────
//
    lsl w19, w19, #1                        // len <<= 1
    cmp w19, w20                                  
    b.gt fft_outer_loop_end                 // if len > n
fft_outer_loop_body_2:
//
//  ─────── IN ────────
//
//  w19 - len
//  x21 - data
//  x23 - SP + 112
//  x24 - SP + 120
//  d0 - 1
//  d10 - invert val
//  d11 - 2 * PI
//
//  ─────── OUT ───────
//
//  [SP + 112] = cos(PI * invert val)
//  [SP + 120] = sin(PI * invert val)
//
//  x2 - len * 4
//  x3 - len * 4 + 4
//  w4 - len / 2
//  w5 - i
//  x6 - data
//  x7 - len * 8
//
//  s7 - wlen.im
//  s16 - wlen.re
//
//  ───────────────────
//
    scvtf d0, w19                           // d0 = len
    fdiv d0, d11, d0                        // d0 = 2 * PI / len
    mov x1, x23                             // cos_out = SP + 112
    mov x0, x24                             // sin_out = SP + 120
    fmul d0, d0, d10                        // x = PI * invert val
    bl __sincos                             // void __sincos(double x, double* sin_out, double* cos_out)
    ldr d16, [sp, #112]                     // d16 = cos
    fcvt s16, d16                           // wlen.re = (float)d16 = cos
    ldr d7, [sp, #120]                      // d7 = sin
    fcvt s7, d7                             // wlen.im = (float)d7 = sin
    asr w4, w19, #1                         // w4 = len >> 1 = len / 2
    sbfiz x7, x19, #3, #32                  // x7 = (int32_t)(len & 0xFFFFFFFF) << 3 = len * 8
    mov x6, x21                             // x6 = data
    sbfiz x2, x4, #3, #32                   // x2 = (int32_t)(w4 & 0xFFFFFFFF) << 3 = len * 4
    add x3, x2, #0x4                        // x3 = len * 4 + 4
    mov w5, #0x0
    b fft_middle_loop_body
not_enough_elements:
//
//  ─────── IN ────────
//
//  w1 - n
//  w25 - invert
//
//  ─────── OUT ───────
//
//  ───────────────────
//
    cbz w25, fft_iterative_neon_end         // if invert == 0
    cmp w1, #0x0
    b.le fft_iterative_neon_end             // if n <= 0
    b invert                                // if n > 0
fft_outer_loop_end: 
//
//  ─────── IN ────────
//
//  w25 - invert
//
//  ─────── OUT ───────
//
//  x19 - ?
//  x20 - ?
//  x23 - ?
//  x24 - ?
//  d8 - ?
//  d9 - ?
//  d10 - ?
//  d11 - ?
//
//  ───────────────────
//
    cbz w25, skip_invert                    // if invert == 0
    ldp x19, x20, [sp, #16]
    ldp x23, x24, [sp, #48]
    ldp d8, d9, [sp, #80]
    ldp d10, d11, [sp, #96]
invert:
//
//  ─────── IN ────────
//
//  x21 - data
//  w22 - n
//
//  ─────── OUT ───────
//
//  x0 - data
//  x21 - data + n * 8
//  
//  s1 - n
//
//  ───────────────────
//
    mov x0, x21                             // x0 = data
    add x21, x21, w22, sxtw#3               // data += n * 8
    scvtf s1, w22                           // s1 = (float)n
invert_loop:
//
//  ─────── IN ────────
//
//  x0 - data
//  x21 - data + n * 8 
//
//  s1 - n
//
//  ─────── OUT ───────
//
//  ───────────────────
//
    ldr s0, [x0]                            // s0 = data[].re
    fdiv s0, s0, s1                         // s0 /= n
    str s0, [x0]                            // data[].re = s0
    ldr s0, [x0, #4]                        // s0 = data[].im
    fdiv s0, s0, s1                         // s0 /= n
    str s0, [x0, #4]                        // data[].im = s0
    add x0, x0, #0x8                        // data += 8
    cmp x21, x0
    b.ne invert_loop                        // if data != data_end
fft_iterative_neon_end:
//
//  ─────── IN ────────
//
//
//  ─────── OUT ───────
//
//  x21 - ?
//  x22 - ?
//  x25 - ?
//  x29 - ?
//  x30 - ?
//
//  ───────────────────
//
    ldp x21, x22, [sp, #32]
    ldr x25, [sp, #64]
    ldp x29, x30, [sp], #128
    ret
skip_invert:
//
//  ─────── IN ────────
//
//
//  ─────── OUT ───────
//
//  x19 - ?
//  x20 - ?
//  x23 - ?
//  x24 - ?
//  
//  d8 - ?
//  d9 - ?
//  d10 - ?
//  d11 - ?
//
//  ───────────────────
//
    ldp x19, x20, [sp, #16]
    ldp x23, x24, [sp, #48]
    ldp d8, d9, [sp, #80]
    ldp d10, d11, [sp, #96]
    b fft_iterative_neon_end


multiply_two_complex:
