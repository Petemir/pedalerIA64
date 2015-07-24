global flanger_asm

; input 
; rdi: *dataBuffIn
; rsi: *dataBuffOut
; rdx: *dataBuffEffect
; rcx: *dataBuffIndex
; r8: framesRead
; r9: channels
; xmm0: amp

locales:
%define index_4 [rbp-4]
%define index_3 [rbp-8]
%define index_2 [rbp-12]
%define index_1 [rbp-16]
%define dataBuffIn rdi
%define dataBuffOut rsi
%define dataBuffEffect edx
%define dataBuffIndex ecx
%define eff_i eax

; rax: contador de frames

section .text
    flanger_asm:
    push rbp        ; convención C
    mov rbp, rsp
    sub rsp, 32
    push rbx
    push r12
    push r13
    push r14
    push r15

    shufps xmm0, xmm0, 0x00 ; xmm0 = amp | amp | amp | amp |
    xor rax, rax    ; rax = eff_i

    cmp r8, 2
    je cycle_stereo

    cycle_mono:
    cmp r8, 0  ; ya recorrí todo el buffer?
    je fin
    cmp r8, 4  ; caso borde: cantidad de frames restantes menor que 4
    jl odd_frames_mono

    ; dataBuffEffect[eff_i] = dataBuffIn[i];
    ; dataBuffOut[out_i] = dataBuffEffect[eff_i];  
    ; dataBuffOut[out_i+1] = dataBuffEffect[eff_i]*amp + amp*dataBuffEffect[dataBuffIndex[eff_i]];

    movaps xmm1, [dataBuffIn]   ; xmm1 = dataBuffIn[i..i+3]
    movaps xmm2, xmm1           ; xmm2 = xmm1
    movaps [dataBuffEffect+eff_i*4], xmm1  ; dataBuffEffect[eff_i..eff_i+3] = dataBuffIn[i..i+3]
    
    ; busco los indices para el efecto
    movaps xmm3, [dataBuffIndex+eff_i*4] ; xmm3 = dataBuffIndex[eff_i]
    movd ebx, xmm3              ; ebx = dataBuffIndex[eff_i+0]
    mov r10d, [dataBuffEffect+4*ebx]
    mov index_1, r10d
    shufps xmm3, xmm3, 00111001b ; xmm3 = 1 | 2 | 3 | 0 |

    movd ebx, xmm3              ; ebx = dataBuffIndex[eff_i+1]
    mov r10d, [dataBuffEffect+4*ebx]
    mov index_2, r10d
    shufps xmm3, xmm3, 00111001b ; xmm3 = 2 | 3 | 0 | 1 |

    movd ebx, xmm3              ; ebx = dataBuffIndex[eff_i+2]
    mov r10d, [dataBuffEffect+4*ebx]
    mov index_3, r10d
    shufps xmm3, xmm3, 00111001b ; xmm3 = 3 | 0 | 1 | 2 |

    movd ebx, xmm3              ; ebx = dataBuffIndex[eff_i+3]
    mov r10d, [dataBuffEffect+4*ebx]
    mov index_4, r10d

    movaps xmm3, index_1    
    addps xmm2, xmm3    ; xmm2 = dataBuffEffect[eff_i] + dataBuffEffect[dataBuffIndex[eff_i]]
    mulps xmm2, xmm0    ; xmm2 = amp * (dataBuffEffect[eff_i] + dataBuffEffect[dataBuffIndex[eff_i]])

    movaps xmm4, xmm1       ; Trabajo en xmm4 con dataBuffIn para no ensuciar
    punpckldq xmm4, xmm2    ; xmm4 = dBIn[0] | amp*(dBEff[0]+dBEff[dBIndex[eff_i]]) | dBIn[1] | amp*(dBEff[1]+dBEff[dBIndex[eff_i+1]]) 
    movaps [rsi], xmm4      ; dataBuffOut = xmm4
    add rsi, 16
    movaps xmm4, xmm1
    punpckhdq xmm4, xmm2
    movaps [rsi], xmm4
    add rsi, 16

    add dataBuffIn, 16  ; nos movemos 16 bytes (4 float) en el puntero de entrada
    add eff_i, 4        ; nos movemos 4 posiciones en el índice del puntero de efecto
    sub r8, 4           ; se procesaron 4 frames

    jmp cycle_mono

    odd_frames_mono:
    ; frames restantes, proceso de a un sólo float
    ; dataBuffOut[i] = dataBuffIn[i]
    ; dataBuffOut[i+1] = dataBuffEffect[i]

    movss xmm1, [dataBuffIn]   ; xmm1 = dataBuffIn[i..i+3]
    movss xmm2, xmm1           ; xmm2 = xmm1
    movss [dataBuffEffect+eff_i*4], xmm1  ; dataBuffEffect[eff_i] = dataBuffIn[i]
    
    ; busco los indices para el efecto
    mov ebx, [dataBuffIndex+eff_i*4] ; ebx = dataBuffIndex[eff_i]
    movd xmm3, [dataBuffEffect+4*ebx]

    addss xmm2, xmm3  ; xmm2 = dataBuffEffect[eff_i] + dataBuffEffect[dataBuffIndex[eff_i]]
    mulss xmm2, xmm0    ; xmm2 = amp * (dataBuffEffect[eff_i] + dataBuffEffect[dataBuffIndex[eff_i]])

    movss [rsi], xmm1
    add rsi, 4
    movss [rsi], xmm2
    add rsi, 4

    add dataBuffIn, 4   ; nos movemos 4 bytes (1 float) en el puntero de entrada
    add eff_i, 1        ; nos movemos 1 posicion en el indice del puntero de efecto
    sub r8, 1           ; se proceso 1 frame
    cmp r8, 0
    je fin

    jmp odd_frames_mono

    cycle_stereo:
    cmp rcx, 0  ; ya recorrí todo el buffer?
    je fin
    cmp rcx, 2  ; caso borde: cantidad de frames restantes igual a 2 (siempre es par, por ser stereo)
    je remaining_frames_stereo

    ; dataBuffOut[i] = 0.5*(dataBuffIn[i]+dataBuffIn[i+1])
    movaps xmm0, [rdi]  ; xmm0 = dataBuffIn[0] | dataBuffIn[1] | dataBuffIn[2] | dataBuffIn[3] |
    mulps xmm0, xmm2    ; xmm0 = 0.5*dataBuffIn[0..3]

    movaps xmm3, xmm0   ; xmm3 = xmm0
    shufps xmm3, xmm3, 0x70   ; xmm3 = 0.5*dataBuffIn[1] | 0.5*dataBuffIn[3] | .. | .. |
    shufps xmm0, xmm0, 0x20   ; xmm0 = 0.5*dataBuffIn[0] | 0.5*dataBuffIn[2] | .. | .. |
    addps xmm0, xmm3          ; xmm0 = 0.5*(dataBuffIn[0+1]) | 0.5*(dataBuffIn[2+3])

    ; dataBuffOut[i+1] = 0.5*(dataBuffEffect[i]+dataBuffEffect[i+1])
    movaps xmm1, [rdx]  ; xmm1 = dataBuffEffect[0..3]
    mulps xmm1, xmm2    ; xmm1 = 0.5*dataBuffEffect[0..3]
    
    movaps xmm3, xmm1
    shufps xmm3, xmm3, 0x70   ; xmm3 = dataBuffEffect[1] | dataBuffEffect[3] | .. | .. |
    shufps xmm1, xmm1, 0x20   ; xmm1 = dataBuffEffect[0] | dataBuffEffect[2] | .. | .. |
    addps xmm1, xmm3          ; xmm1 = 0.5*(dataBuffEffect[0+1]) | 0.5*(dataBuffEffect[2+3])

    punpckhdq xmm0, xmm1      ; xmm0 = 0.5*(dataBuffIn[0+1]) | 0.5*(dataBuffEffect[0+1]) | 0.5*(dataBuffIn[2+3]) | 0.5*(dataBuffEffect[2+3])
    movaps [rsi], xmm0

    ; dataBuffEffect[i] = dataBuffIn[i] * decay
    movss xmm1, [r8]        ; xmm1 = decay | - | - | - |
    shufps xmm1, xmm1, 0x00 ; xmm1 = decay | decay | decay | decay |
    movaps xmm0, [rdi]     ; xmm0 = dataBuffIn
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
    add rsp, 32
    pop rbp
    ret