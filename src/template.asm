global function_asm

; input enteros
; rdi: *dataBuffIn
; rsi: *dataBuffOut
; rdx: *dataBuffEffect
; rcx:
; r8:
; r9:
; input floats
; xmm0:
; xmm1:
; xmm2:
; xmm3:

section .text
    push rbp        ; convención C
    mov rbp, rsp
    ;sub rsp, N    ; espacio para variables locales
    push rbx
    push r12
    push r13
    push r14
    push r15

    cycle:
    cmp rcx, 0
    je fin
    cmp rcx, 4
    cmp rcx, 2
    je ?;

    ; return en rax, o xmm0
    fin:
    pop r15
    pop r14
    pop r13
    pop r12
    ;add rsp, N
    pop rbx
    pop rbp
    ret
