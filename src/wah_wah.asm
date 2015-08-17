global wah_wah_asm

; input enteros
; rdi: *dataBuffIn
; rsi: *dataBuffOut
; rdx: *dataBuffMod
; rcx: length
; r8: channels
; r9: *yh
; [rbp+16]: yb
; [rbp+24]: yl

; input floats
; xmm0: q1

%define dataBuffIn rdi
%define dataBuffOut rsi
%define dataBuffMod rdx
%define length ecx
%define channels r8d
%define yh [r9]

%define tmpreg r10
%define cyclecount r11

%define yb_stack [rbp+16]
%define yl_stack [rbp+24]
%define stereotmp [rbp-8]

%define qs xmm0
%define input xmm1
%define yhs xmm4
%define ybs xmm5
%define yls xmm6
%define fcs xmm7

; AUXILIARES ;
%define tmp xmm2
%define outtmp xmm9

; CONSTANTES ;
%define halves xmm3
%define tenths xmm8

section .text
    wah_wah_asm:
        push rbp        ; convención C
        mov rbp, rsp
        sub rsp, 8    ; espacio para variables locales
        push rbx
        push r12
        push r13
        push r14
        push r15

    ; setup ;
        mov eax, 0X3F000000         ; 0.5
        movd halves, eax
        shufps halves, halves, 0x00 ; halves = |0.5|0.5|0.5|0.5|

        mov eax, 0x3DCCCCCD         ; 0.1
        movd tenths, eax
        shufps tenths, tenths, 0x00 ; tenths = |0.1|0.1|0.1|0.1|

        shufps qs, qs, 0x00         ; qs = |q1|q1|q1|q1|

        movd yhs, yh            ; yhs = |x|x|x|yh|
        shufps yhs, yhs, 0x00   ; yhs = |yh|yh|yh|yh|

        mov tmpreg, yb_stack
        movd ybs, [tmpreg]      ; ybs = |x|x|x|yb|
        shufps ybs, ybs, 0x00   ; ybs = |yb|yb|yb|yb|

        mov tmpreg, yl_stack
        movd yls, [tmpreg]      ; yls = |x|x|x|yl|
        shufps yls, yls, 0x00   ; yls = |yl|yl|yl|yl|

    cycle:
        cmp channels, 2
        je input_stereo

    input_mono:
        cmp length, 0
        je fin
        cmp length, 4
        jl remaining_frames_mono_input

        movaps input, [dataBuffIn]

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
        ;; calculo efecto ;;
        movaps fcs, [dataBuffMod]   ; fcs = |fc|fc|fc|fc|
        movaps yhs, input   ; yhs = |input|input|input|input|

        mov cyclecount, 4
        cycle_four:
            subss yhs, yls      ; yhs = input - yl
            movss tmp, ybs      ; tmp = yb
            mulss tmp, qs       ; tmp = q1*yb
            subss yhs, tmp      ; yhs = input-yl-q1*yb

            movss tmp, yhs      ; tmp = yh
            mulss tmp, fcs      ; tmp = fc*yh
            addss ybs, tmp      ; ybs = fc*yh+yb

            movss tmp, ybs      ; tmp = yb
            mulss tmp, fcs      ; tmp = fc*yb
            addss yls, tmp      ; yls = fc*yb+yl

            movss tmp, ybs      ; tmp = yb
            mulss tmp, tenths   ; tmp = 0.1*yb

            sub cyclecount, 1
            cmp cyclecount, 0
            je continue_cycle_common

            shufps yhs, yhs, 00111001b
;            shufps yls, yls, 00111001b
;            shufps ybs, ybs, 00111001b
            shufps fcs, fcs, 00111001b
            shufps tmp, tmp, 00111001b

            jmp cycle_four
    continue_cycle_common:
        shufps tmp, tmp, 00111001b
        movaps outtmp, input
        punpckldq outtmp, tmp
        movaps [dataBuffOut], outtmp
        add dataBuffOut, 16

        movaps outtmp, input
        punpckhdq outtmp, tmp
        movaps [dataBuffOut], outtmp
        add dataBuffOut, 16

        add dataBuffIn, 16
        add dataBuffMod, 16
        sub length, 4
        jmp cycle

    remaining_frames:
        cmp channels, 2
        je remaining_frames_stereo_input

    remaining_frames_mono_input:
        cmp length, 0
        je fin

        movss input, [dataBuffIn]
        jmp remaining_frames_cycle_common

    remaining_frames_stereo_input:
        cmp length, 0
        je fin

        movq input, [dataBuffIn]    ; input = |...|...|dataBuffIn[1]|dataBuffIn[0]|
        mulps input, halves   ; input = 0.5*input
        movaps tmp, input       ; tmp = input
        shufps tmp, tmp, 0x01 ; tmp = |...|...|...|dataBuffIn[1]

        addss input, tmp      ; tmp = |...|...|...|0.5*dataBuffIn[0+1]

        add dataBuffIn, 4
        sub length, 1

    remaining_frames_cycle_common:
        movss fcs, [dataBuffMod]
        movss yhs, input

        subss yhs, yls      ; yhs = input - yl
        movss tmp, ybs      ; tmp = yb
        mulss tmp, qs       ; tmp = q1*yb
        subss yhs, tmp      ; yhs = input-yl-q1*yb

        movss tmp, yhs      ; tmp = yh
        mulss tmp, fcs      ; tmp = fc*yh
        addss ybs, tmp      ; ybs = fc*yh+yb

        movss tmp, ybs      ; tmp = yb
        mulss tmp, fcs      ; tmp = fc*yb
        addss yls, tmp      ; yls = fc*yb+yl

        movss tmp, ybs      ; tmp = yb
        mulss tmp, tenths   ; tmp = 0.1*yb

        punpckldq input, tmp
        movq [dataBuffOut], input

        add dataBuffOut, 8
        add dataBuffMod, 4
        add dataBuffIn, 4
        sub length, 1
        jmp remaining_frames

    ; return en rax, o xmm0
    fin:
    movss yh, yhs

    mov tmpreg, yb_stack
    movss [tmpreg], ybs

    mov tmpreg, yl_stack
    movss [tmpreg], yls

    pop r15
    pop r14
    pop r13
    pop r12
    add rsp, 8
    pop rbx
    pop rbp
    ret
