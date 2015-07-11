global delay_simple_asm

; input 
; rdi: *dataBuffIn
; rsi: *dataBuffOut
; rdx: *dataBuffEffect
; rcx: framesRead
; r8: decay
; r9: channels

section .text
    delay_simple_asm:
    push rbp        ; convención C
    mov rbp, rsp
    push rbx
    push r12
    push r13
    push r14
    push r15

    cmp r9, 1
    je cycle_mono

    mov eax,  0x3F000000 ; 0.5 en eax
    movd xmm2, eax
    shufps xmm2, xmm2, 0x00 ; xmm2 = 0.5 | 0.5 | 0.5 | 0.5 |

    jmp cycle_stereo

; TODO -> FIJARSE SI SE PUEDE CORREGIR PARA QUE HAGA LO MISMO TANTO EN STEREO COMO MONO, Y SOLO VARIE LO QUE SE GUARDA EN EL BUFFER EFECTO
    cycle_mono:
    cmp rcx, 0  ; ya recorrí todo el buffer?
    je fin
    cmp rcx, 4  ; caso borde: cantidad de frames restantes menor que 4
    jl odd_frames_mono

    ; dataBuffOut[i] = dataBuffIn[i]
    ; dataBuffOut[i+1] = dataBuffEffect[i]

    movaps xmm0, [rdi]  ; xmm0 = dataBuffIn[0..3]
    movaps xmm1, [rdx]  ; xmm1 = dataBuffEffect[0..3]

    movaps xmm2, xmm0   ; trabajo en xmm2 para no ensuciar xmm0
    punpckldq xmm2, xmm1; mezclo en xmm2 las partes bajas de xmm2 y xmm1
    movaps [rsi], xmm2  ; dataBuffOut = xmm2 = dataBuffIn[0]+dataBuffEffect[0]+dataBuffIn[1]+dataBuffEffect[1]
    add rsi, 16         ; me muevo 16 bytes (4 floats) en el buffer de salida
    movaps xmm2, xmm0
    punpckhdq xmm2, xmm1; mezclo en xmm2 las partes altas de xmm2 y xmm1
    movaps [rsi], xmm2  ; dataBuffOut = xmm2 = dataBuffIn[2]+dataBuffEffect[2]+dataBuffIn[3]+dataBuffEffect[3]
    add rsi, 16

    ; dataBuffEffect[i] = dataBuffIn[i] * decay
    movss xmm1, [r8]        ; xmm1 = decay | - | - | - |
    shufps xmm1, xmm1, 0x00 ; xmm1 = decay | decay | decay | decay |
    mulps xmm0, xmm1       ; xmm0 = dataBuffIn * decay
    movaps [rdx], xmm0     ; dataBuffEffect = xmm0

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
    cmp rcx, 2  ; caso borde: cantidad de frames restantes igual a 2 (siempre es par, por ser stereo)
    je remaining_frames_stereo

    movaps xmm4, [rdi]  ; xmm4 = dataBuffIn[0] | dataBuffIn[1] | dataBuffIn[2] | dataBuffIn[3] |

    ; dataBuffOut[i] = 0.5*(dataBuffIn[i]+dataBuffIn[i+1])
    movaps xmm0, xmm4   ; xmm0 = xmm4
    mulps xmm0, xmm2    ; xmm0 = 0.5*dataBuffIn[0..3]

    movaps xmm3, xmm0         ; xmm3 = xmm0
    shufps xmm3, xmm3, 0xD0   ; xmm3 = 0.5*dataBuffIn[1] | 0.5*dataBuffIn[3] | .. | .. |
    shufps xmm0, xmm0, 0x80   ; xmm0 = 0.5*dataBuffIn[0] | 0.5*dataBuffIn[2] | .. | .. |
    addps xmm0, xmm3          ; xmm0 = 0.5*(dataBuffIn[0+1]) | 0.5*(dataBuffIn[2+3])

    ; dataBuffOut[i+1] = 0.5*(dataBuffEffect[i]+dataBuffEffect[i+1])
    movaps xmm1, [rdx]  ; xmm1 = dataBuffEffect[0..3]
    mulps xmm1, xmm2    ; xmm1 = 0.5*dataBuffEffect[0..3]
    
    movaps xmm3, xmm1         ; xmm3 = xmm1
    shufps xmm3, xmm3, 0xD0   ; xmm3 = dataBuffEffect[1] | dataBuffEffect[3] | .. | .. |
    shufps xmm1, xmm1, 0x80   ; xmm1 = dataBuffEffect[0] | dataBuffEffect[2] | .. | .. |
    addps xmm1, xmm3          ; xmm1 = 0.5*(dataBuffEffect[0+1]) | 0.5*(dataBuffEffect[2+3])

    punpckhdq xmm0, xmm1      ; xmm0 = 0.5*(dataBuffIn[0+1]) | 0.5*(dataBuffEffect[0+1]) | 0.5*(dataBuffIn[2+3]) | 0.5*(dataBuffEffect[2+3])
    movaps [rsi], xmm0

    ; dataBuffEffect[i] = dataBuffIn[i] * decay
    movss xmm1, [r8]        ; xmm1 = decay | - | - | - |
    shufps xmm1, xmm1, 0x00 ; xmm1 = decay | decay | decay | decay |
    movaps xmm0, xmm4     ; xmm0 = dataBuffIn
    mulps xmm0, xmm1       ; xmm0 = dataBuffIn[0..3] * decay
    movaps [rdx], xmm0     ; dataBuffEffect = xmm0

    sub rcx,4   ; se procesaron 4 frames
    add rdi,16   ; nos movemos 16 bytes (4 float) en cada puntero
    add rsi,16
    add rdx,16

    jmp cycle_stereo

    remaining_frames_stereo:
    ; frames restantes, procesamos dos floats (canal izquierdo y derecho)
    ; dataBuffOut[i] = 0.5*(dataBuffIn[i]+dataBuffIn[i+1])
    
    movlps xmm0, [rdi]  ; xmm0 = dataBuffIn[0]+dataBuffIn[1]
    mulps xmm0, xmm2    ; xmm0 = 0.5*dataBuffIn[0,1]

    movaps xmm3, xmm0
    shufps xmm3, xmm3, 0x10 ; xmm3 = dataBuffIn[1] | .. | .. | .. |
    addss xmm0, xmm3    ; xmm0 = 0.5*(dataBuffIn[0+1]) | .. | .. | .. |

    movlps [rsi], xmm0
    add rsi, 4

    ; dataBuffOut[i+1] = 0.5*(dataBuffEffect[i]+dataBuffEffect[i+1])
    movlps xmm1, [rdx]  ; xmm1 = dataBuffEffect[0]+dataBuffEffect[1]
    mulps xmm1, xmm2    ; xmm1= 0.5*dataBuffEffect[0,1]
    
    movaps xmm3, xmm1
    shufps xmm3, xmm3, 0x10 ; xmm3 = dataBuffEffect[1] | .. | .. | .. |
    addss xmm1, xmm3    ; xmm0 = 0.5*(dataBuffEffect[0+1]) | .. | .. | .. |

    movlps [rsi], xmm1
    add rsi, 4

    ; dataBuffEffect[i] = dataBuffIn[i] * decay
    movss xmm0, [r8]        ; xmm0 = decay | - | - | - |
    shufps xmm0, xmm0, 0x00 ; xmm0 = decay | decay | decay | decay |
    movlps xmm1, [rdi]     ; xmm1 = dataBuffIn[0] | dataBuffIn[1] | .. | .. |
    mulps xmm1, xmm0       ; xmm1 = dataBuffIn[0] * decay | dataBuffIn[1] * decay | .. | .. |
    movlps [rdx], xmm1     ; dataBuffEffect = xmm1

    add rdi, 8
    add rdx, 8
    sub rcx, 2

    fin:
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret