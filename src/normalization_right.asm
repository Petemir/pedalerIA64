global normalization_right_asm

; input enteros
; rdi: *dataBuffIn
; rsi: *dataBuffOut
; rdx: *maxRightValue
; rcx: framesRead

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
    movss xmm1, [rdx]        ; xmm1 = | max |  0  |  0  |  0  |
    shufps xmm1, xmm1, 0x00 ; xmm0 = | max | max | max | max |
    punpckldq xmm0, xmm1    ; xmm0 = | max |  1  | max |  1  |

    cycle:
    cmp rcx, 0
    je fin
    cmp rcx, 2
    je remaining_frames

    movaps xmm2, [rdi]
    jmp normalization

    remaining_frames:
    movlps xmm2, [rdi]

    normalization:
    mulps xmm2, xmm0        ; normalizo el canal derecho
    movaps [rsi], xmm2

    add rdi, 8      ; nos movemos por lo menos 8 bytes (2 float) en la entrada
    add rsi, 8
    sub rcx, 2

    cmp rcx, 0
    je fin

    add rdi, 8      ; como no eran los ultimos dos frames, recorrimos 4 en total y seguimos
    add rsi, 8
    sub rcx, 2

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
