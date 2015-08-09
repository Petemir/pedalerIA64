global bitcrusher_asm

; bitcrusher_asm(dataBuffIn, dataBuffOut, framesRead, bits, freq, inFileStr.samplerate, &phasor);

; input enteros/*floats
; rdi:  *dataBuffIn
; rsi:  *dataBuffOut
; rdx:  framesRead
; rcx:  *steps
; r8:   *normFreq
; r9:   *phasor
; []
%define dataBuffIn rdi
%define dataBuffOut rsi
%define framesRead rdx
%define step rcx
%define phasor r8
%define last r9
%define channels [rbp+16]
%define stereotmp [rbp-8]

%define phasors xmm2
%define multipliers xmm1
%define sumFreqs xmm0
%define halves xmm3
%define ones xmm4
%define input xmm5
%define steps xmm6
%define tmp xmm7
%define cmpflag xmm8
%define phasorsint xmm9
%define lasts xmm10

%define one ebx

_numbers12 dq 0x400000003f800000
_numbers34 dq 0x4080000040400000

section .text
    bitcrusher_asm:
        push rbp        ; convención C
        mov rbp, rsp
        sub rsp, 12
        push rbx
        push r12
        push r13
        push r14
        push r15

        ; setup de registros que se van a usar en todos los ciclos
        movss phasors, [phasor]
        shufps phasors, phasors, 0x00   ; phasors = | phasor | phasor | phasor | phasor |

        movss steps, [step]
        shufps steps, steps, 0x00       ; steps = | step | step | step | step |

        movss lasts, [last]
        shufps lasts, lasts, 0x00       ; lasts = | last | last | last | last |

        shufps sumFreqs, sumFreqs, 0x00 ; sumFreqs = | normFreq | normFreq | normFreq | normFreq |

        movhps multipliers, [_numbers34]    ; multipliers = | 4 | 3 | x | x |
        movlps multipliers, [_numbers12]    ; multipliers = | 4 | 3 | 1 | 2 |
        mulps sumFreqs, xmm1                ; sumFreqs = | normFreq*4 | normFreq*3 | normFreq*2 | normFreq*1 |

        mov eax, 0X3F000000         ; 0.5
        movd halves, eax
        shufps halves, halves, 0x00 ; halves = | 0.5 | 0.5 | 0.5 | 0.5 |

        mov eax, channels
        mov ebx, 0x3f800000
        movd ones, ebx
        shufps ones, ones, 0x00      ; ones =  | 1 | 1 | 1 | 1 |

    cycle:
        cmp eax, 2
        je input_stereo

    input_mono:
        cmp framesRead, 0
        je fin
        cmp framesRead, 4
        jl remaining_frames_mono

        movaps input, [dataBuffIn]      ; quedan más de 4 frames
        jmp cycle_common

    input_stereo:
        cmp framesRead, 0
        je fin
        cmp framesRead, 8
        jl remaining_frames_stereo    ; proceso de a dos frames

        ; quedan 8 frames por lo menos, así que puedo juntar los 2 canales en 4 frames y hacer el ciclo común

        movaps input, [dataBuffIn]  ; input = |dataBuffIn[3]|dataBuffIn[2]|dataBuffIn[1]|dataBuffIn[0]|
        mulps input, halves         ; input = 0.5*dataBuffIn[0..3]
        movaps tmp, input               ; tmp = input
        shufps tmp, tmp, 10001101b      ; tmp = |...|...|0.5*dataBuffIn[3]|0.5*dataBuffIn[1]|
        shufps input, input, 1101000b ; input = |...|...|0.5*dataBuffIn[2]|0.5*dataBuffIn[0]|
        addps input, tmp              ; input = |...|...|0.5*dataBuffIn[2+3]|0.5*dataBuffIn[0+1]|
        movlpd stereotmp, input     ; stereotmp = |0.5*dataBuffIn[2+3]|0.5*dataBuffIn[0+1]|

        add dataBuffIn, 16
        sub framesRead, 4

        movaps input, [dataBuffIn]  ; input = |dataBuffIn[7]|dataBuffIn[6]|dataBuffIn[5]|dataBuffIn[4]|
        mulps input, halves         ; input = 0.5*dataBuffIn[4..7]
        movaps tmp, input               ; tmp = input
        shufps tmp, tmp, 10001101b      ; tmp = |...|...|0.5*dataBuffIn[7]|0.5*dataBuffIn[5]|
        shufps input, input, 1101000b ; input = |...|...|0.5*dataBuffIn[6]|0.5*dataBuffIn[4]|
        addps input, tmp              ; input = |...|...|0.5*dataBuffIn[7+6]|0.5*dataBuffIn[5+4]|
        movlhps input, input          ; input = |0.5*dataBuffIn[7+6]|0.5*dataBuffIn[5+4]|...|...|
        movlpd input, stereotmp       ; input = 0.5*dataBuffIn[|(7+6)|(5+4)|(3+2)|(1+0)|]

    cycle_common:
        addps phasors, sumFreqs     ; phasor = phasor + |normFreq*4|normFreq*3|normFreq*2|normFreq*1|

        movaps cmpflag, ones        ; cmpflag = | 1 | 1 | 1 | 1 |
        cmpps cmpflag, phasors, 2   ; cmpflag = (1.0 <= phasors)
        ptest cmpflag, cmpflag
        jnz set_lasts                ; si hay alguna comparacion distinta de 0, seteo nuevo last

    continue_cycle:
        movaps tmp, input
        punpckldq tmp, lasts
        movaps [dataBuffOut], tmp
        add dataBuffOut, 16

        movaps tmp, input
        punpckhdq tmp, lasts
        movaps [dataBuffOut], tmp
        add dataBuffOut, 16

        shufps phasors, phasors, 0xFF

        add dataBuffIn, 16
        sub framesRead, 4
        jmp cycle

    set_lasts:
        movaps tmp, input       ; tmp = input
        divps tmp, steps        ; tmp = input/step
        addps tmp, halves       ; tmp = (input/step + 0.5)
        roundps tmp, tmp, 0x01  ; tmp = floor(input/step + 0.5) // TODO -> UNCOMMENT
        mulps tmp, steps        ; tmp = step*floor(dataBuffEffect[0..3]/step+0.5)

        movaps phasorsint, phasors
        andps phasorsint, cmpflag   ; me quedo con los phasors >= 1.0
        roundps phasorsint, phasorsint, 01b     ; tomo floor de esos phasors
        subps phasors, phasorsint   ; me quedo con la parte decimal en phasors

        movss [last], tmp         ; last = tmp
        movss lasts, tmp
        shufps lasts, lasts, 0x00   ; lasts = | last | last | last | last |
        jmp continue_cycle

    remaining_frames_mono:
        cmp framesRead, 0
        je fin

        movss input, [dataBuffIn]
        punpckldq input, lasts
        movq [dataBuffOut], input

        add dataBuffIn, 4
        add dataBuffOut, 8
        sub framesRead, 1
        jmp remaining_frames_mono

    remaining_frames_stereo:
        cmp framesRead, 0
        je fin

        movd input, [dataBuffIn]    ; input = |...|...|dataBuffIn[1]|dataBuffIn[0]|
        mulps input, halves   ; input = 0.5*input
        movaps tmp, input       ; tmp = input
        shufps tmp, tmp, 0x01 ; tmp = |...|...|...|dataBuffIn[1]

        addss tmp, input      ; tmp = |...|...|...|0.5*dataBuffIn[0+1]

        punpckldq tmp, lasts
        movd [dataBuffOut], tmp

        add dataBuffIn, 8
        add dataBuffOut, 8
        sub framesRead, 2
        jmp remaining_frames_stereo

    fin:
    ; tengo que mover el ultimo phasors hacia [phasor]
    shufps phasors, phasors, 0xFF
    movss [phasor], phasors

    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    add rsp, 12
    pop rbp
    ret
