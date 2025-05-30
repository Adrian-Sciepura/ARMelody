.global fft_iterative
.extern __sincos

fft_iterative:
    stp x29, x30, [sp, #-128]!
    mov x29, sp
    stp x21, x22, [sp, #32]
    str x25, [sp, #64]
    mov x21, x0
    mov w22, w1
    and w25, w2, #0xff
    cmp w1, #0x1
    b.le fft_iterative_1
    stp x19, x20, [sp, #16]
    stp x23, x24, [sp, #48]
    stp d8, d9, [sp, #80]
    stp d10, d11, [sp, #96]
    asr w3, w1, #1
    add x2, x0, #0x8
    mov w1, #0x0
    mov w20, #0x1
    b fft_iterative_2
fft_iterative_5:
    mov w0, w3
    b fft_iterative_3
fft_iterative_7:
    add w20, w20, #0x1
    add x2, x2, #0x8
    cmp w22, w20
    b.eq fft_iterative_4
fft_iterative_2:
    tst w3, w1
    b.eq fft_iterative_5
    mov w0, w3
fft_iterative_6:
    eor w1, w1, w0
    asr w0, w0, #1
    tst w1, w0
    b.ne fft_iterative_6
fft_iterative_3:
    eor w1, w1, w0
    cmp w1, w20
    b.le fft_iterative_7
    ldr s1, [x2]
    ldr s0, [x2, #4]
    sbfiz x0, x1, #3, #32
    add x4, x21, x0
    ldr x5, [x21, w1, sxtw#3]
    str x5, [x2]
    str s1, [x21, x0]
    str s0, [x4, #4]
    b fft_iterative_7
fft_iterative_4:
    sxtb w0, w25
    cmp w0, #0x0
    fmov d10, #-1.000000000000000000e+00
    fmov d0, #1.000000000000000000e+00
    fcsel d10, d10, d0, ne
    mov w19, #0x2
    add x24, sp, #0x78
    add x23, sp, #0x70
    ldr x0, =0x401921fb54442d18
    fmov d11, x0
    movi v9.2s, #0x0
    fmov s8, #1.000000000000000000e+00
    b fft_iterative_8
fft_iterative_11:
    mov x0, x6
    fmov s2, s9
    fmov s1, s8
    mov w1, #0x0 // #0
fft_iterative_9:
    ldr s4, [x0]
    ldr s3, [x0, #4]
    ldr s6, [x0, x2]
    ldr s0, [x0, x3]
    fmul s5, s6, s1
    fmul s17, s0, s2
    fsub s5, s5, s17
    fmul s0, s0, s1
    fmul s6, s6, s2
    fadd s0, s0, s6
    fadd s6, s4, s5
    str s6, [x0]
    fadd s6, s3, s0
    str s6, [x0, #4]
    fsub s4, s4, s5
    str s4, [x0, x2]
    fsub s3, s3, s0
    str s3, [x0, x3]
    fmov s0, s1
    fmul s1, s16, s1
    fmul s3, s7, s2
    fsub s1, s1, s3
    fmul s0, s7, s0
    fmul s2, s16, s2
    fadd s2, s0, s2
    add w1, w1, #0x1
    add x0, x0, #0x8
    cmp w1, w4
    b.lt fft_iterative_9
fft_iterative_12:
    add w5, w5, w19
    add x6, x6, x7
    cmp w5, w20
    b.ge fft_iterative_10
fft_iterative_14:
    cmp w19, #0x1
    b.gt fft_iterative_11
    b fft_iterative_12
fft_iterative_10:
    lsl w19, w19, #1
    cmp w19, w20
    b.gt fft_iterative_13
fft_iterative_8:
    scvtf d0, w19
    fdiv d0, d11, d0
    mov x1, x23
    mov x0, x24
    fmul d0, d0, d10
    bl __sincos
    ldr d16, [sp, #112]
    fcvt s16, d16
    ldr d7, [sp, #120]
    fcvt s7, d7
    asr w4, w19, #1
    sbfiz x7, x19, #3, #32
    mov x6, x21
    sbfiz x2, x4, #3, #32
    add x3, x2, #0x4
    mov w5, #0x0
    b fft_iterative_14
fft_iterative_1:
    cbz w25, fft_iterative_15
    cmp w1, #0x0
    b.le fft_iterative_15
    b fft_iterative_16
fft_iterative_13:
    cbz w25, fft_iterative_17
    ldp x19, x20, [sp, #16]
    ldp x23, x24, [sp, #48]
    ldp d8, d9, [sp, #80]
    ldp d10, d11, [sp, #96]
fft_iterative_16:
    mov x0, x21
    add x21, x21, w22, sxtw#3
    scvtf s1, w22
fft_iterative_18:
    ldr s0, [x0]
    fdiv s0, s0, s1
    str s0, [x0]
    ldr s0, [x0, #4]
    fdiv s0, s0, s1
    str s0, [x0, #4]
    add x0, x0, #0x8
    cmp x21, x0
    b.ne fft_iterative_18
fft_iterative_15:
    ldp x21, x22, [sp, #32]
    ldr x25, [sp, #64]
    ldp x29, x30, [sp], #128
    ret
fft_iterative_17:
    ldp x19, x20, [sp, #16]
    ldp x23, x24, [sp, #48]
    ldp d8, d9, [sp, #80]
    ldp d10, d11, [sp, #96]
    b fft_iterative_15
