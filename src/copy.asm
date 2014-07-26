global copy_asm

; input
; rdi: *buffer_in;
; rsi: *buffer_out; 
; rdx: framesRead

section .text
    copy_asm:
    push rbp        ; convención C
    mov rbp, rsp
    push rbx
    push r12
    push r13
    push r14
    push r15

    cycle:
    cmp rdx,0   ; ya recorrí todo el buffer?
    je fin
    cmp rdx, 1  ; caso borde: cantidad de frames impar
    je odd_frames

    movapd xmm0, [rdi]
    movapd [rsi], xmm0

    add rdi,16
    add rsi,16

    inc rax
    sub rdx,2  ; recorrí 128bits más
    jmp cycle

    odd_frames:
    movhpd xmm0, [rdi]
    movhpd [rsi], xmm0
    
    fin:
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret