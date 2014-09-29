global delay_asm

; input 
; rdi: *dataBuffIn
; rsi: *dataBuffOut
; rdx: dataBuffEffect
; rcx: framesRead
; r8: decay
; r9: channels

section .text
    delay_asm:
    push rbp        ; convención C
    mov rbp, rsp
    push rbx
    push r12
    push r13
    push r14
    push r15

    cmp r9, 2
    je cycle_stereo

    cycle_mono:
    cmp rcx, 0  ; ya recorrí todo el buffer?
    je fin
    cmp rcx, 1  ; caso borde: cantidad de frames impar
    je odd_frames_mono

    ; dataBuffOut[i] = dataBuffIn[i] + dataBuffEffect
    movapd xmm0, [rdi]  ; xmm0 = dataBuffIn
    movapd xmm1, [rdx]  ; xmm1 = dataBuffEffect
    addpd xmm0, [rdx]   ; xmm0 = dataBuffIn + dataBuffEffect
    movapd [rsi], xmm0  ; dataBuffOut = xmm0

    ; dataBuffEffect[i] = dataBuffIn[i] * decay
    movhpd xmm0, [r8]  ; xmm0 = decay | -
    movlpd xmm0, [r8]  ; xmm0 = decay | decay
    movapd xmm1, [rdi] ; xmm1 = dataBuffIn
    mulpd xmm1, xmm0   ; xmm1 = dataBuffIn * decay
    movapd [rdx], xmm1    ; dataBuffEffect = xmm1

    sub rcx,2   ; se procesaron 2 frames
    add rdi,16  ; nos movemos 16 bytes (2 doubles) en cada puntero
    add rsi,16
    add rdx,16

    jmp cycle_mono

    odd_frames_mono:
    ; frame restante, procesamos un sólo double
    ; dataBuffOut[i] = dataBuffIn[i] + dataBuffEffect
    movlpd xmm0, [rdi]  ; xmm0 = dataBuffIn
    movlpd xmm1, [rdx]  ; xmm1 = dataBuffEffect
    addsd xmm0, [rdx]   ; xmm0 = dataBuffIn + dataBuffEffect
    movlpd [rsi], xmm0  ; dataBuffOut = xmm0

    ; dataBuffEffect[i] = dataBuffIn[i] * decay
    movlpd xmm0, [r8]  ; xmm0 = decay | -
    movlpd xmm1, [rdi] ; xmm1 = dataBuffIn
    mulsd xmm1, xmm0   ; xmm1 = dataBuffIn * decay
    movlpd [rdx], xmm1    ; dataBuffEffect = xmm1

    jmp fin

    cycle_stereo:
    cmp rcx, 0  ; ya recorrí todo el buffer?
    je fin
    cmp rcx, 1  ; caso borde: cantidad de frames impar
    je odd_frames_mono

    ; dataBuffOut[i] = dataBuffIn[i] + dataBuffEffect
    movapd xmm0, [rdi]  ; xmm0 = dataBuffIn
    movapd xmm1, [rdx]  ; xmm1 = dataBuffEffect
    addpd xmm0, [rdx]   ; xmm0 = dataBuffIn + dataBuffEffect
    movapd [rsi], xmm0  ; dataBuffOut = xmm0

    ; dataBuffEffect[i] = dataBuffIn[i] * decay
    movhpd xmm0, [r8]  ; xmm0 = decay | -
    movlpd xmm0, [r8]  ; xmm0 = decay | decay
    movapd xmm1, [rdi] ; xmm1 = dataBuffIn
    mulpd xmm1, xmm0   ; xmm1 = dataBuffIn * decay
    movapd [rdx], xmm1    ; dataBuffEffect = xmm1

    sub rcx,2   ; se procesaron 2 frames
    add rdi,16  ; nos movemos 16 bytes (2 doubles) en cada puntero
    add rsi,16
    add rdx,16

    jmp cycle_stereo

    odd_frames_stereo:
    ; frame restante, procesamos un sólo double
    ; dataBuffOut[i] = dataBuffIn[i] + dataBuffEffect
    movlpd xmm0, [rdi]  ; xmm0 = dataBuffIn
    movlpd xmm1, [rdx]  ; xmm1 = dataBuffEffect
    addsd xmm0, [rdx]   ; xmm0 = dataBuffIn + dataBuffEffect
    movlpd [rsi], xmm0  ; dataBuffOut = xmm0

    ; dataBuffEffect[i] = dataBuffIn[i] * decay
    movlpd xmm0, [r8]  ; xmm0 = decay | -
    movlpd xmm1, [rdi] ; xmm1 = dataBuffIn
    mulsd xmm1, xmm0   ; xmm1 = dataBuffIn * decay
    movlpd [rdx], xmm1    ; dataBuffEffect = xmm1

    fin:
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret