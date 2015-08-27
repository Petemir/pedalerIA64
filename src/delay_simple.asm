global delay_simple_asm

; input
; rdi: *dataBuffIn
; rsi: *dataBuffOut
; rdx: *dataBuffEffect
; rcx: framesRead
; r8: channels
; xmm0: decay

%define length rcx
%define channels r8
%define dataBuffIn rdi
%define dataBuffOut rsi
%define dataBuffEffect rdx

%define stereotmp [rbp-16]

%define decay xmm0
%define input xmm1
%define effect xmm2
%define halves xmm3
%define tmp xmm4
%define tmp2 xmm5

section .text
    delay_simple_asm:
        push rbp        ; convención C
        mov rbp, rsp
        sub rsp, 16
        push rbx
        push r12
        push r13
        push r14
        push r15

    ; setup ;
        mov eax,  0x3F000000 ; 0.5 en eax
        movd halves, eax
        shufps halves, halves, 0x00 ; halves = |0.5|0.5|0.5|0.5|

        shufps decay, decay, 0x00   ; decay = |decay|decay|decay|decay|

    cycle:
        cmp channels, 2
        je input_stereo

    input_mono:
        cmp length, 0  ; ya recorrí todo el buffer?
        je fin
        cmp length, 4  ; caso borde: cantidad de frames restantes menor que 4
        jl remaining_frames_mono_input

        movaps input, [dataBuffIn]  ; input = dataBuffIn[i..i+3]
        jmp cycle_common

    input_stereo:
        cmp length, 0
        je fin
        cmp length, 8
        jl remaining_frames_stereo_input

        movaps input, [dataBuffIn]  ; input = |dataBuffIn[3]|dataBuffIn[2]|dataBuffIn[1]|dataBuffIn[0]|
        mulps input, halves         ; input = 0.5*dataBuffIn[0..3]
        movaps tmp, input               ; tmp = input
        shufps tmp, tmp, 10001101b      ; tmp = |...|...|0.5*dataBuffIn[3]|0.5*dataBuffIn[1]|
        shufps input, input, 1101000b ; input = |...|...|0.5*dataBuffIn[2]|0.5*dataBuffIn[0]|
        addps input, tmp              ; input = |...|...|0.5*dataBuffIn[2+3]|0.5*dataBuffIn[0+1]|
        movlpd stereotmp, input     ; stereotmp = |0.5*dataBuffIn[2+3]|0.5*dataBuffIn[0+1]|

        add dataBuffIn, 16
        movaps input, [dataBuffIn]  ; input = |dataBuffIn[7]|dataBuffIn[6]|dataBuffIn[5]|dataBuffIn[4]|
        mulps input, halves         ; input = 0.5*dataBuffIn[4..7]
        movaps tmp, input               ; tmp = input
        shufps tmp, tmp, 10001101b      ; tmp = |...|...|0.5*dataBuffIn[7]|0.5*dataBuffIn[5]|
        shufps input, input, 1101000b ; input = |...|...|0.5*dataBuffIn[6]|0.5*dataBuffIn[4]|
        addps input, tmp              ; input = |...|...|0.5*dataBuffIn[7+6]|0.5*dataBuffIn[5+4]|
        movlhps input, input          ; input = |0.5*dataBuffIn[7+6]|0.5*dataBuffIn[5+4]|...|...|
        movlpd input, stereotmp       ; input = 0.5*dataBuffIn[|(7+6)|(5+4)|(3+2)|(1+0)|]

        sub length, 4

    cycle_common:
        movaps effect, [dataBuffEffect]  ; xmm1 = dataBuffEffect[0..3]

        movaps tmp, input   ; trabajo en tmp para no ensuciar input
        punpckldq tmp, effect ; mezclo en tmp las partes bajas de input y effect
        movaps [dataBuffOut], tmp  ; dataBuffOut = dataBuffIn[0]+dataBuffEffect[0]+dataBuffIn[1]+dataBuffEffect[1]
        add dataBuffOut, 16         ; me muevo 16 bytes (4 floats) en el buffer de salida

        movaps tmp, input
        punpckhdq tmp, effect ; mezclo en tmp las partes altas de input y effect
        movaps [dataBuffOut], tmp  ; dataBuffOut = dataBuffIn[2]+dataBuffEffect[2]+dataBuffIn[3]+dataBuffEffect[3]
        add dataBuffOut, 16

        ; dataBuffEffect[i] = dataBuffIn[i] * decay
        mulps input, decay       ; input = dataBuffIn * decay
        movaps [dataBuffEffect], input      ; dataBuffEffect = input

        add dataBuffEffect, 16
        add dataBuffIn,16   ; nos movemos 16 bytes (4 float) en cada puntero
        sub length, 4    ; se procesaron 4 frames

        jmp cycle

    remaining_frames:
        cmp channels, 2
        je remaining_frames_stereo_input

    remaining_frames_mono_input:
        cmp length, 0
        je fin

        ; frames restantes, proceso de a un sólo float
        movss input, [dataBuffIn]   ; xmm1 = dataBuffIn[i..i+3]
        jmp remaining_frames_cycle_common

    remaining_frames_stereo_input:
        cmp length, 0
        je fin

        movq input, [dataBuffIn]    ; input = |...|...|dataBuffIn[1]|dataBuffIn[0]|
        mulps input, halves         ; input = 0.5*input
        movaps tmp, input           ; tmp = input
        shufps tmp, tmp, 0x01       ; tmp = |...|...|...|dataBuffIn[1]

        addss input, tmp      ; input = |...|...|...|0.5*dataBuffIn[0+1]

        add dataBuffIn, 4
        sub length, 1

    remaining_frames_cycle_common:
        ; frames restantes, proceso de a un sólo float
        ; dataBuffOut[i] = dataBuffIn[i]
        ; dataBuffOut[i+1] = dataBuffEffect[i]

        movss input, [dataBuffIn]       ; xmm0 =|-|-|-|dataBuffIn[0]|
        movss effect, [dataBuffEffect]  ; xmm1 =|-|-|-|dataBuffEffect[0]|

        movss [dataBuffOut], input  ; dataBuffOut[i] = dataBuffIn[i]
        add dataBuffOut, 4
        movss [dataBuffOut], effect ; dataBuffOut[i+1] = dataBuffEffect[i]
        add dataBuffOut, 4

        ; dataBuffEffect[i] = dataBuffIn[i] * decay
        mulss input, decay  ; xmm0 = dataBuffIn * decay | - | - | - |
        movss [dataBuffEffect], input  ; dataBuffEffect = xmm1

        add dataBuffIn, 4
        add dataBuffEffect, 4
        sub length, 1  ; se proceso 1 frame

        jmp remaining_frames

    fin:
        pop r15
        pop r14
        pop r13
        pop r12
        add rsp, 16
        pop rbx
        pop rbp
        ret
