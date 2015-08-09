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

%define dataBuffIn rdi
%define dataBuffOut rsi
%define length ?
%define channels ?

%define input ?
%define tmp ?
%define halves ?


section .text
    function_asm:
        push rbp        ; convención C
        mov rbp, rsp
        ;sub rsp, N    ; espacio para variables locales
        push rbx
        push r12
        push r13
        push r14
        push r15

    ; setup ;
        mov eax, 0X3F000000         ; 0.5
        movd halves, eax
        shufps halves, halves, 0x00 ; halves = | 0.5 | 0.5 | 0.5 | 0.5 |

    cycle:
        cmp eax, 2
        je input_stereo

    input_mono:
        cmp length, 0
        je fin
        cmp length, 4
        jl remaining_frames_mono

        movaps input, [dataBuffIn]

        jmp cycle_common

    input_stereo:
        cmp length, 0
        je fin
        cmp length, 8
        jl remaining_frames_stereo

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
        ;; calculo efecto ;;

        add dataBuffIn, 16
        add dataBuffOut, 16
        sub length, 4
        jmp cycle

    remaining_frames_mono:
        cmp length, 0
        je fin

        ;; calculo efecto mono 1 frame ;;

        add dataBuffIn, 4
        add dataBuffOut, 4
        sub length, 1
        jmp remaining_frames_mono

    remaining_frames_stereo
        cmp length, 0
        je fin

        movd input, [dataBuffIn]    ; input = |...|...|dataBuffIn[1]|dataBuffIn[0]|
        mulps input, halves   ; input = 0.5*input
        movaps tmp, input       ; tmp = input
        shufps tmp, tmp, 0x01 ; tmp = |...|...|...|dataBuffIn[1]

        ;; calculo efecto mono 1 frame 2 floats ;;

        addss tmp, input      ; tmp = |...|...|...|0.5*dataBuffIn[0+1]

        add dataBuffIn, 8
        add dataBuffOut, 8
        sub framesRead, 2
        jmp remaining_frames_stereo

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
