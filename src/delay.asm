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
    cmp rcx, 4  ; caso borde: cantidad de frames restantes menor que 4
    jl odd_frames_mono

    ; dataBuffOut[i] = dataBuffIn[i]
    ; dataBuffOut[i+1] = dataBuffEffect[i]

    movups xmm0, [rdi]  ; xmm0 = dataBuffIn[0..3]
    movups xmm1, [rdx]  ; xmm1 = dataBuffEffect[0..3]

    movups xmm2, xmm0   ; trabajo en xmm2 para no ensuciar xmm0
    punpckldq xmm2, xmm1; mezclo en xmm2 las partes altas de xmm2 y xmm1
    movups [rsi], xmm2  ; dataBuffOut = xmm2 = dataBuffIn[0]+dataBuffEffect[0]+dataBuffIn[1]+dataBuffEffect[1]
    add rsi, 16         ; me muevo 16 bytes (4 floats) en el buffer de salida
    movups xmm2, xmm0
    punpckhdq xmm2, xmm1; mezclo en xmm2 las partes bajas de xmm2 y xmm1
    movups [rsi], xmm2  ; dataBuffOut = xmm2 = dataBuffIn[2]+dataBuffEffect[2]+dataBuffIn[3]+dataBuffEffect[3]
    add rsi, 16

    ; dataBuffEffect[i] = dataBuffIn[i] * decay

    movss xmm0, [r8]        ; xmm0 = decay | - | - | - |
    shufps xmm0, xmm0, 0x00 ; xmm0 = decay | decay | decay | decay |
    movaps xmm1, [rdi]     ; xmm1 = dataBuffIn
    mulps xmm1, xmm0       ; xmm1 = dataBuffIn * decay
    movaps [rdx], xmm1     ; dataBuffEffect = xmm1

    sub rcx,4    ; se procesaron 4 frames
    add rdi,16   ; nos movemos 16 bytes (4 float) en cada puntero
    add rdx,16

    jmp cycle_mono

    odd_frames_mono:
    ; frames restantes, proceso de a un sólo float
    ; dataBuffOut[i] = dataBuffIn[i]
    ; dataBuffOut[i+1] = dataBuffEffect[i]
    
    movss xmm0, [rdi]  ; xmm0 = dataBuffIn[0] | - | - | - |
    movss xmm1, [rdx]  ; xmm1 = dataBuffEffect[0] | - | - | - |
    
    movss [rsi], xmm0  ; dataBuffOut[i] = dataBuffIn[i]
    add rsi, 4
    movss [rsi], xmm1 ; dataBuffOut[i+1] = dataBuffEffect[i]
    add rsi, 4

    ; dataBuffEffect[i] = dataBuffIn[i] * decay
    movss xmm0, [r8]  ; xmm0 = decay | - | - | - |
    movss xmm1, [rdi] ; xmm1 = dataBuffIn | - | - | - |
    mulss xmm0, xmm1  ; xmm0 = dataBuffIn * decay | - | - | - |
    movss [rdx], xmm0 ; dataBuffEffect = xmm1

    add rdi, 4
    add rdx, 4
    sub rcx,1  ; se proceso 1 frame
    cmp rcx,0
    je fin

    jmp odd_frames_mono

    cycle_stereo:
    cmp rcx, 0  ; ya recorrí todo el buffer?
    je fin
    cmp rcx, 1  ; caso borde: cantidad de frames impar
    je odd_frames_mono
;    cmp rcx, 4  ; caso borde: cantidad de frames impar
;    jl odd_frames_mono

    ; HACER PROMEDIO DE IZQ+DER EN CASO DE MONO
    ; dataBuffOut[i] = dataBuffIn[i] + dataBuffEffect
;double
    movapd xmm0, [rdi]  ; xmm0 = dataBuffIn
    movapd xmm1, [rdx]  ; xmm1 = dataBuffEffect
    addpd xmm0, [rdx]   ; xmm0 = dataBuffIn + dataBuffEffect
    movapd [rsi], xmm0  ; dataBuffOut = xmm0
;float
    ;movaps xmm0, [rdi]  ; xmm0 = dataBuffIn
    ;movaps xmm1, [rdx]  ; xmm1 = dataBuffEffect
    ;addps xmm0, [rdx]   ; xmm0 = dataBuffIn + dataBuffEffect
    ;movaps [rsi], xmm0  ; dataBuffOut = xmm0

    ; dataBuffEffect[i] = dataBuffIn[i] * decay
;double
    movhpd xmm0, [r8]  ; xmm0 = decay | -
    movlpd xmm0, [r8]  ; xmm0 = decay | decay
    movapd xmm1, [rdi] ; xmm1 = dataBuffIn
    mulpd xmm1, xmm0   ; xmm1 = dataBuffIn * decay
    movapd [rdx], xmm1    ; dataBuffEffect = xmm1
;float
    ;movhps xmm0, [r8]  ; xmm0 = decay | -
    ;movlps xmm0, [r8]  ; xmm0 = decay | decay
    ;movaps xmm1, [rdi] ; xmm1 = dataBuffIn
    ;mulps xmm1, xmm0   ; xmm1 = dataBuffIn * decay
    ;movaps [rdx], xmm1    ; dataBuffEffect = xmm1

    sub rcx,2   ; se procesaron 2 frames
    add rdi,16  ; nos movemos 16 bytes (2 doubles) en cada puntero
;    sub rcx,4   ; se procesaron 4 frames
;    add rdi,16   ; nos movemos 16 bytes (4 float) en cada puntero
    add rsi,16
    add rdx,16

    jmp cycle_stereo

    odd_frames_stereo:
    ; frame restante, procesamos un sólo double
    ; dataBuffOut[i] = dataBuffIn[i] + dataBuffEffect
;double
    movlpd xmm0, [rdi]  ; xmm0 = dataBuffIn
    movlpd xmm1, [rdx]  ; xmm1 = dataBuffEffect
    addsd xmm0, [rdx]   ; xmm0 = dataBuffIn + dataBuffEffect
    movlpd [rsi], xmm0  ; dataBuffOut = xmm0
;float
    ;movlps xmm0, [rdi]  ; xmm0 = dataBuffIn
    ;movlps xmm1, [rdx]  ; xmm1 = dataBuffEffect
    ;addss xmm0, [rdx]   ; xmm0 = dataBuffIn + dataBuffEffect
    ;movlps [rsi], xmm0  ; dataBuffOut = xmm0

    ; dataBuffEffect[i] = dataBuffIn[i] * decay
;double
    movlpd xmm0, [r8]  ; xmm0 = decay | -
    movlpd xmm1, [rdi] ; xmm1 = dataBuffIn
    mulsd xmm1, xmm0   ; xmm1 = dataBuffIn * decay
    movlpd [rdx], xmm1    ; dataBuffEffect = xmm1
;float
    ;movlps xmm0, [r8]  ; xmm0 = decay | -
    ;movlps xmm1, [rdi] ; xmm1 = dataBuffIn
    ;mulss xmm1, xmm0   ; xmm1 = dataBuffIn * decay
    ;movlps [rdx], xmm1    ; dataBuffEffect = xmm1

;    sub rcx,1
;    cmp rcx, 0
;    jne odd_frames_stereo

    fin:
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret