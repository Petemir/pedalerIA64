global flanger_asm

; input
; rdi: *dataBuffIn
; rsi: *dataBuffOut
; rdx: *dataBuffEffect
; rcx: *dataBuffIndex
; r8: framesRead
; r9: channels
; xmm0: amp

%define dataBuffIn rdi
%define dataBuffOut rsi
%define dataBuffEffect edx
%define dataBuffIndex ecx
%define length r8
%define channels r9

%define index_4 [rbp-4]
%define index_3 [rbp-8]
%define index_2 [rbp-12]
%define index_1 [rbp-16]
%define stereotmp [rbp-24]

%define eff_i eax

%define tmp_reg r10d
%define tmp_idx ebx

%define amp xmm0

%define input xmm1
%define tmp xmm2
%define tmp2 xmm3
;%define stereotmp xmm4
%define halves xmm4
%define indices xmm5
; rax: contador de frames

section .text
    flanger_asm:
        push rbp        ; convención C
        mov rbp, rsp
        sub rsp, 16
        push rbx
        push r12
        push r13
        push r14
        push r15

    ; setup ;
        shufps xmm0, xmm0, 0x00 ; xmm0 = amp | amp | amp | amp |
        mov eax,  0x3F000000 ; 0.5 en eax
        movd halves, eax
        shufps halves, halves, 0x00 ; xmm5 = 0.5 | 0.5 | 0.5 | 0.5 |

        xor eff_i, eff_i    ; eff_i = rax = 0

    cycle:
        cmp channels, 2
        je input_stereo

    input_mono:
        cmp length, 0  ; ya recorrí todo el buffer?
        je fin
        cmp length, 4  ; caso borde: cantidad de frames restantes menor que 4
        jl remaining_frames_mono_input

        movaps input, [dataBuffIn]   ; input = dataBuffIn[i..i+3]
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
        sub length, 4

        movaps input, [dataBuffIn]  ; input = |dataBuffIn[7]|dataBuffIn[6]|dataBuffIn[5]|dataBuffIn[4]|
        mulps input, halves         ; input = 0.5*dataBuffIn[4..7]
        movaps tmp, input               ; tmp = input
        shufps tmp, tmp, 10001101b      ; tmp = |...|...|0.5*dataBuffIn[7]|0.5*dataBuffIn[5]|
        shufps input, input, 1101000b ; input = |...|...|0.5*dataBuffIn[6]|0.5*dataBuffIn[4]|
        addps input, tmp              ; input = |...|...|0.5*dataBuffIn[7+6]|0.5*dataBuffIn[5+4]|
        movlhps input, input          ; input = |0.5*dataBuffIn[7+6]|0.5*dataBuffIn[5+4]|...|...|
        movlpd input, stereotmp       ; input = 0.5*dataBuffIn[|(7+6)|(5+4)|(3+2)|(1+0)|]


    cycle_common:
        movaps [dataBuffEffect+eff_i*4], input  ; dataBuffEffect[eff_i..eff_i+3] = dataBuffIn[i..i+3]

        ; busco los indices para el efecto
        movaps indices, [dataBuffIndex+eff_i*4] ; xmm3 = dataBuffIndex[eff_i]
        movd tmp_idx, indices              ; ebx = dataBuffIndex[eff_i+0]
        mov tmp_reg, [dataBuffEffect+4*tmp_idx]
        mov index_1, tmp_reg

        shufps indices, indices, 00111001b ; xmm3 =|0|3|2|1|
        movd tmp_idx, indices              ; ebx = dataBuffIndex[eff_i+1]
        mov tmp_reg, [dataBuffEffect+4*tmp_idx]
        mov index_2, tmp_reg

        shufps indices, indices, 00111001b ; xmm3 = |1|0|3|2|
        movd tmp_idx, indices              ; ebx = dataBuffIndex[eff_i+2]
        mov tmp_reg, [dataBuffEffect+4*tmp_idx]
        mov index_3, tmp_reg

        shufps indices, indices, 00111001b ; xmm3 = |2|1|0|3|
        movd tmp_idx, indices              ; ebx = dataBuffIndex[eff_i+3]
        mov tmp_reg, [dataBuffEffect+4*tmp_idx]
        mov index_4, tmp_reg

        movaps tmp2, index_1    ; tmp2 = dataBuffEffect[dataBuffIndex[eff_i]]
        movaps tmp, input       ; tmp = xmm1 = dataBuffEffect[eff_i]
        addps tmp, tmp2         ; tmp = dataBuffEffect[eff_i] + dataBuffEffect[dataBuffIndex[eff_i]]
        mulps tmp, amp         ; tmp = amp * (dataBuffEffect[eff_i] + dataBuffEffect[dataBuffIndex[eff_i]])

        movaps tmp2, input       ; Trabajo en tmp2 con input para no ensuciar
        punpckldq tmp2, tmp    ; tmp2 = dBIn[0] | amp*(dBEff[0]+dBEff[dBIndex[eff_i]]) | dBIn[1] | amp*(dBEff[1]+dBEff[dBIndex[eff_i+1]])
        movaps [dataBuffOut], tmp2      ; dataBuffOut = xmm4
        add dataBuffOut, 16

        movaps tmp2, input
        punpckhdq tmp2, tmp
        movaps [dataBuffOut], tmp2
        add dataBuffOut, 16

        add dataBuffIn, 16  ; nos movemos 16 bytes (4 float) en el puntero de entrada
        add eff_i, 4        ; nos movemos 4 posiciones en el índice del puntero de efecto
        sub length, 4           ; se procesaron 4 frames

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
        movss tmp, input           ; xmm2 = xmm1
        movss [dataBuffEffect+eff_i*4], input  ; dataBuffEffect[eff_i] = dataBuffIn[i]

        ; busco los indices para el efecto
        mov tmp_idx, [dataBuffIndex+eff_i*4] ; ebx = dataBuffIndex[eff_i]
        movd tmp2, [dataBuffEffect+4*tmp_idx]

        addss tmp, tmp2  ; xmm2 = dataBuffEffect[eff_i] + dataBuffEffect[dataBuffIndex[eff_i]]
        mulss tmp, amp    ; xmm2 = amp * (dataBuffEffect[eff_i] + dataBuffEffect[dataBuffIndex[eff_i]])

        punpckldq input, tmp
        movq [dataBuffOut], input

        add dataBuffOut, 8
        add dataBuffIn, 4   ; nos movemos 4 bytes (1 float) en el puntero de entrada
        add eff_i, 1        ; nos movemos 1 posicion en el indice del puntero de efecto
        sub length, 1           ; se proceso 1 frame

        jmp remaining_frames

    fin:
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    add rsp, 16
    pop rbp
    ret
