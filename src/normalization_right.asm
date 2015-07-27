global normalization_right_asm

; input enteros
; rdi: *dataBuffIn
; rsi: *dataBuffOut
; rdx: *maxRightValue
; rcx: framesRead
; r8:
; r9:
; input floats
; xmm0:
; xmm1:
; xmm2:
; xmm3:

section .text
    normalization_right_asm:
    push rbp        ; convención C
    mov rbp, rsp
    push rbx
    push r12
    push r13
    push r14
    push r15

    mov eax, 0x3f800000     ; 1 en IEEE-754 Single Precision
    movd xmm0, eax
    shufps xmm0, xmm0, 0x00 ; xmm0 = |  1  |  1  |  1  |  1  |
    movd xmm1, [rdx]        ; xmm1 = | max |  0  |  0  |  0  |
    shufps xmm1, xmm1, 0x00 ; xmm0 = | max | max | max | max |
    punpckldq xmm0, xmm1    ; xmm0 = | max |  1  | max |  1  |

    cycle:
    cmp rcx, 0
    je fin
    cmp rcx, 2
    je fin

    movaps xmm2, [rdi]
    mulps xmm2, xmm0        ; normalizo el canal derecho
    movaps [rsi], xmm2

    sub rcx, 4
    add rdi, 16
    add rsi, 16

    jmp cycle

    ; return en rax, o xmm0
    fin:
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret
