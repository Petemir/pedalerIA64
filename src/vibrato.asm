global vibrato_asm

; input enteros
; rdi: *dataBuffIn
; rsi: *dataBuffOut
; rdx: *circularBuffer
; rcx: *bufferIndex
; r8:  *circularBufferHead
; r9:  *circularBufferEnd
; input floats
; xmm0: delay
; xmm1:
; xmm2:
; xmm3:
; [rbp+16]: framesRead
; [rbp+24]: channels

%define dataBuffIn rdi
%define dataBuffOut rsi
%define circularBuffer edx
%define bufferIndex ecx
%define circularBufferHead r8
%define circularBufferEnd r9

%define head ebx

%define length_mem [rbp+16]
%define channels_mem [rbp+24]
%define length r10d
%define channels r11d
%define tmpreg r12d

%define index_4 [rbp-4]
%define index_3 [rbp-8]
%define index_2 [rbp-12]
%define index_1 [rbp-16]
%define index_8 [rbp-20]
%define index_7 [rbp-24]
%define index_6 [rbp-28]
%define index_5 [rbp-32]
%define stereotmp [rbp-40]

%define input xmm1
%define tmp xmm2
%define halves xmm3
%define circularBufferIndices xmm4
%define sub_index xmm5
%define ones xmm6
%define ns xmm7
%define fracs xmm8
%define circularBufferEnds xmm9
%define cmpflag xmm10
%define tmp2 xmm11
%define tmp3 xmm12
%define ones_float xmm13

_numbers10 dq 0x3f80000000000000
_numbers32 dq 0x4040000040000000

section .text
    vibrato_asm:
        push rbp        ; convención C
        mov rbp, rsp
        sub rsp, 40    ; espacio para variables locales
        push rbx
        push r12
        push r13
        push r14
        push r15

    ; setup ;
        mov eax, 0X3F000000         ; 0.5
        movd halves, eax
        shufps halves, halves, 0x00 ; halves = |0.5|0.5|0.5|0.5|

        mov channels, channels_mem
        mov length, length_mem

        mov eax, 0x40400000 ; 3.0
        movd sub_index, eax
        shufps sub_index, sub_index, 0x00   ; inc_index = |3.0|3.0|3.0|3.0|

        mov eax, 0x3f800000 ; 1.0
        movd ones, eax
        shufps ones, ones, 0x00   ; ones = |1.0|1.0|1.0|1.0|

        ; mov head, 7960

        movd circularBufferEnds, [circularBufferEnd]
        shufps circularBufferEnds, circularBufferEnds, 0x00
        cvtdq2ps circularBufferEnds, circularBufferEnds ; (float)circularBufferEnds

        movss circularBufferIndices, circularBufferEnds
        subss circularBufferIndices, ones
        ;[circularBufferHead]
        shufps circularBufferIndices, circularBufferIndices, 0x00
        ;cvtdq2ps circularBufferIndices, circularBufferIndices ; (float)circularBufferIndices

        movhps tmp, [_numbers32]
        movlps tmp, [_numbers10]
        subps circularBufferIndices, tmp ; circularBufferIndices = |head-3.0|head-2.0|head-1.0|head|

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
        movaps tmp, circularBufferIndices   ; lo uso en continue_cycle_zero
        subps circularBufferIndices, ones   ; dataBuffEffectHead--

        movaps cmpflag, circularBufferIndices
        pxor tmp3, tmp3
        cmpps cmpflag, tmp3, 1 ; cmpflag = (circularBufferIndices < 0)
        ptest cmpflag, cmpflag
        jz continue_cycle_zero

        andps cmpflag, circularBufferEnds
        addps circularBufferIndices, cmpflag

    continue_cycle_zero:
        movaps tmp2, input
        cvtps2dq tmp, tmp   ; tmp = (int) tmp = (int)indices

        movd head, tmp
        movd eax, tmp2
        mov [circularBuffer+4*head], eax

        shufps tmp, tmp, 00111001b  ; tmp = |0|3|2|1|
        shufps tmp2, tmp2, 00111001b; tmp2 = |0|3|2|1|
        movd head, tmp
        movd eax, tmp2
        mov [circularBuffer+4*head], eax

        shufps tmp, tmp, 00111001b  ; tmp = |1|0|3|2|
        shufps tmp2, tmp2, 00111001b; tmp2 = |1|0|3|2|
        movd head, tmp
        movd eax, tmp2
        mov [circularBuffer+4*head], eax

        shufps tmp, tmp, 00111001b  ; tmp = |2|1|0|3|
        shufps tmp2, tmp2, 00111001b; tmp2 = |2|1|0|3|
        movd head, tmp
        movd eax, tmp2
        mov [circularBuffer+4*head], eax

        movaps fracs, [bufferIndex]   ; fracs = tap
        roundps ns, fracs, 01b        ; ns = floor(tap) = n
        subps fracs, ns               ; fracs = tap-floor(tap) = parte fraccionaria

        movaps tmp, circularBufferIndices ; tmp = dataBuffEffectHead
        addps tmp, ns      ; tmp = dataBuffEffectHead+n
        movaps tmp2, tmp   ; tmp2 = dataBuffEffectHead+n
        addps tmp, ones    ; tmp = dataBuffEffectHead+n+1

    continue_cycle_one:
        movaps cmpflag, circularBufferEnds
        cmpps cmpflag, tmp, 2 ; cmpflag = circularBufferEnd <= dataBuffEffectHead+n+1
        ptest cmpflag, cmpflag
        jz modulus_two

        modulus_one:
            andps cmpflag, circularBufferEnds
            subps tmp, cmpflag  ; tmp = (dataBuffEffectHead+n+1) % dataBuffEffectEnd

        modulus_two:
            movaps cmpflag, circularBufferEnds
            cmpps cmpflag, tmp2, 2 ; cmpflag = circularBufferEnd <= dataBuffEffectHead+n
            ptest cmpflag, cmpflag
            jz continue_cycle_two

            andps cmpflag, circularBufferEnds
            subps tmp2, cmpflag ; tmp2 = (dataBuffEffectHead+n) % dataBuffEffectEnd

    ; hasta acá, ya tengo los índices para la interpolación lineal ;
    continue_cycle_two:
        cvtps2dq tmp, tmp ; (int) tmp
        movd eax, tmp
        mov tmpreg, [circularBuffer+4*eax]
        mov index_1, tmpreg

        shufps tmp, tmp, 00111001b ; tmp = |0|3|2|1|
        movd eax, tmp
        mov tmpreg, [circularBuffer+4*eax]
        mov index_2, tmpreg

        shufps tmp, tmp, 00111001b ; tmp = |1|0|3|2|
        movd eax, tmp
        mov tmpreg, [circularBuffer+4*eax]
        mov index_3, tmpreg

        shufps tmp, tmp, 00111001b ; tmp = |2|1|0|3|
        movd eax, tmp
        mov tmpreg, [circularBuffer+4*eax]
        mov index_4, tmpreg

        cvtps2dq tmp2, tmp2 ; (int)tmp2
        movd eax, tmp2
        mov tmpreg, [circularBuffer+4*eax]
        mov index_5, tmpreg

        shufps tmp2, tmp2, 00111001b ; tmp2 = |0|3|2|1|
        movd eax, tmp2
        mov tmpreg, [circularBuffer+4*eax]
        mov index_6, tmpreg

        shufps tmp2, tmp2, 00111001b ; tmp2 = |1|0|3|2|
        movd eax, tmp2
        mov tmpreg, [circularBuffer+4*eax]
        mov index_7, tmpreg

        shufps tmp2, tmp2, 00111001b ; tmp2 = |2|1|0|3|
        movd eax, tmp2
        mov tmpreg, [circularBuffer+4*eax]
        mov index_8, tmpreg

        movaps tmp, index_1
        mulps tmp, fracs        ; dataBuffEffect[((dataBuffEffectHead-1)+n+1) % dataBuffEffectEnd+1]*frac

        movaps tmp2, index_5
        movaps tmp3, ones ; tmp3 = |1.0|1.0|1.0|1.0|
        subps tmp3, fracs   ; tmp3 = |1.0-frac|1.0-frac|1.0-frac|1.0-frac|
        mulps tmp2, tmp3    ; dataBuffEffect[((dataBuffEffectHead-1)+n) % dataBuffEffectEnd+1]*(1-frac)

        addps tmp, tmp2    ; interpolacion lineal

        movaps cmpflag, circularBufferIndices
        pxor tmp3, tmp3
        cmpps cmpflag, tmp3, 1 ; cmpflag = (circularBufferIndices < 0)
        ptest cmpflag, cmpflag
        jz finish_cycle

        andps cmpflag, circularBufferEnds
        addps circularBufferIndices, cmpflag

        finish_cycle:
            subps circularBufferIndices, sub_index  ; ya resté uno antes, ahora sólo resto tres en cada índice

            movaps cmpflag, circularBufferIndices
            pxor tmp3, tmp3
            cmpps cmpflag, tmp3, 1 ; cmpflag = (circularBufferIndices < 0)
            ptest cmpflag, cmpflag
            jz continue_finish_cycle

            andps cmpflag, circularBufferEnds
            addps circularBufferIndices, cmpflag

        continue_finish_cycle:
            movaps tmp3, input
            punpckldq tmp3, tmp
            movaps [dataBuffOut], tmp3
            add dataBuffOut, 16

            movaps tmp3, input
            punpckhdq tmp3, tmp
            movaps [dataBuffOut], tmp3
            add dataBuffOut, 16

            add bufferIndex, 16
            add dataBuffIn, 16
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
        mulps input, halves         ; input = 0.5*input
        movaps tmp, input           ; tmp = input
        shufps tmp, tmp, 0x01       ; tmp = |...|...|...|dataBuffIn[1]

        addss input, tmp      ; input = |...|...|...|0.5*dataBuffIn[0+1]

        add dataBuffIn, 4
        sub length, 1

    remaining_frames_cycle_common:
        cvtss2si head, circularBufferIndices
        cmp head, 0
        jge continue_remaining_frames_cycle_common

        movss circularBufferIndices, circularBufferEnds
        cvtss2si head, circularBufferEnds
        sub head, 1
    continue_remaining_frames_cycle_common:
        movd [circularBuffer+4*head], input

        subss circularBufferIndices, ones

        movss fracs, [bufferIndex]
        roundss ns, fracs, 01b
        subss fracs, ns

        movss tmp, circularBufferIndices
        addss tmp, ns
        movss tmp2, tmp
        addss tmp, ones

        movss cmpflag, circularBufferEnds
        cmpss cmpflag, tmp, 2
        ptest cmpflag, cmpflag
        jz continue_cycle_single_one

        andps cmpflag, circularBufferEnds
        subss tmp, cmpflag

    continue_cycle_single_one:
        movss cmpflag, circularBufferEnds
        cmpss cmpflag, tmp2, 2
        ptest cmpflag, cmpflag
        jz continue_cycle_single_two

        andps cmpflag, circularBufferEnds
        subss tmp2, cmpflag

    continue_cycle_single_two:
        cvtps2dq tmp, tmp
        movd eax, tmp
        movss tmp, [circularBuffer+4*eax]

        cvtps2dq tmp2, tmp2
        movd eax, tmp2
        movss tmp2, [circularBuffer+4*eax]

        mulss tmp, fracs

        movss tmp3, ones
        subss tmp3, fracs
        mulss tmp2, tmp3

        addss tmp, tmp2    ; interpolacion lineal

        movss cmpflag, circularBufferIndices
        pxor tmp3, tmp3
        cmpss cmpflag, tmp3, 1
        ptest cmpflag, cmpflag
        jz finish_cycle_single

        andps cmpflag, circularBufferEnds
        addss circularBufferIndices, cmpflag

    finish_cycle_single:
        movss cmpflag, circularBufferIndices
        pxor tmp3, tmp3
        cmpss cmpflag, tmp3, 1 ; cmpflag = (circularBufferIndices < 0)
        ptest cmpflag, cmpflag
        jz continue_finish_cycle_single

        andps cmpflag, circularBufferEnds
        addss circularBufferIndices, cmpflag

    continue_finish_cycle_single:
        punpckldq input, tmp
        movq [dataBuffOut], input

        add dataBuffOut, 8
        add bufferIndex, 4
        add dataBuffIn, 4
        sub length, 1
        jmp remaining_frames

    ; return en rax, o xmm0
    fin:
    cvtps2dq circularBufferIndices, circularBufferIndices
    movss [circularBufferHead], circularBufferIndices  ; guardo el ultimo usado
    pop r15
    pop r14
    pop r13
    pop r12
    add rsp, 40
    pop rbx
    pop rbp
    ret
