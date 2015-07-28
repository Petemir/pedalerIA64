global maxsamp_right_asm

; input enteros
; rdi: *dataBuffIn
; rsi: *maxTemp
; rdx: framesRead

section .text
    maxsamp_right_asm:
    push rbp        ; convención C
    mov rbp, rsp
    push rbx
    push r12
    push r13
    push r14
    push r15

    movd xmm1, [rsi]   ; xmm1[0] = maximo temporal
    shufps xmm1, xmm1, 0x00 ; xmm1 = | max | max | max | max

    cycle:
    cmp rdx, 0
    je fin
    cmp rdx, 2
    je remaining_frames

    movaps xmm3, [rdi]  ; xmm2 = dataBuffIn[0..3]
    jmp comparison

    remaining_frames:
    movq xmm3, [rdi]    ; xmm3 = dataBuffIn[0..1] | 0 | 0 | (entonces nunca van a ser maximos)

    comparison:
    pxor xmm2, xmm2     ; xmm2 = | 0 | 0 | 0 | 0 |
    subps xmm2, xmm3    ; xmm2 = -xmm3
    maxps xmm2, xmm3    ; xmm2 = max(-xmm3,xmm3)

    maxps xmm1, xmm2    ; xmm1 = | max(xmm1[0],xmm3[0]) | max(xmm1[1],xmm3[1]) | max(xmm1[2],xmm3[2]) | max(xmm1[3],xmm3[3])

    add rdi, 8     ; nos movemos por lo menos 8 bytes (2 float) en la entrada
    sub rdx, 2

    cmp rdx, 0
    je fin

    add rdi, 8      ; como no eran los ultimos dos frames, recorrimos 4 en total y seguimos
    sub rdx, 2

    jmp cycle

    fin:
    ; tengo que fijarme cuál es máximo, entre xmm1[1] y xmm1[3]
    movaps xmm0, xmm1
    shufps xmm0, xmm0, 0x01 ; xmm0 = xmm1[1] | ... | ... | ... |
    shufps xmm1, xmm1, 0x03 ; xmm1 = xmm1[3] | ... | ... | ... |

    maxss xmm0, xmm1
    movss [rsi], xmm0

    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret
