global clean_buffer_asm

; input enteros
; rdi: *buffer
; rsi: length

%define buffer rdi
%define length rsi

%define zeros xmm0
section .text
    clean_buffer_asm:
        push rbp        ; convención C
        mov rbp, rsp
        push rbx
        push r12
        push r13
        push r14
        push r15

    ; setup ;
        pxor zeros, zeros ; zeros = |0|0|0|0|

    cycle:
        cmp length, 4
        jl remaining_frames

        movaps [buffer], zeros
        add buffer, 16
        sub length, 4
        jmp cycle

    remaining_frames:
        cmp length, 0
        je fin

        movss [buffer], zeros
        add buffer, 4
        sub length, 1
        jmp remaining_frames

    fin:
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret
