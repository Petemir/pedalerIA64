global bitcrusher_asm

; bitcrusher_asm(dataBuffIn, dataBuffOut, framesRead, bits, freq, inFileStr.samplerate, &phasor);

; input enteros
; rdi:  *dataBuffIn
; rsi:  *dataBuffOut
; rdx:  framesRead
; rcx:  bits
; r8:   freq
; r9:   *phasor

%define dataBuffIn rdi
%define dataBuffOut rsi
%define framesRead rdx
%define step rcx
%define normFreq r8
%define phasor r9

%define phasors xmm0
%define multipliers xmm1
%define sumFreqs xmm2
%define halves xmm3
%define ones xmm4
%define minusones xmm5
%define input xmm6
%define steps xmm7
%define tmp xmm8

%define one ebx

section .text
    push rbp        ; convención C
    mov rbp, rsp
    sub rsp, 4      ; espacio para variables locales
    push rbx
    push r12
    push r13
    push r14
    push r15

    bitcrusher_asm:
    movss phasors, [phasor]
    shufps phasors, phasors, 0x00 ; phasors = | phasor | phasor | phasor | phasor |

    movss steps, [step]
    shufps steps, steps, 0x00   ; steps = | step | step | step | step |

    mov eax, 0X3F000000         ; 0.5
    movd halves, eax
    shufps halves, halves, 0x00 ; halves = | 0.5 | 0.5 | 0.5 | 0.5 |

    mov ebx, 0x3f800000
    movd ones, ebx
    shufps ones, ones, 0x00      ; ones =  | 1 | 1 | 1 | 1 |

    pxor minusones, minusones
    subps minusones, ones   ; minusones = | -1 | -1 | -1 | -1 |

    mov rax, 0x4080000040400000         ; rax = | 4 | 3 |
    movq multipliers, rax               ; multipliers = | x | x | 4 | 3 |
    movlhps multipliers, multipliers    ; multipliers = | 4 | 3 | x | x |
    mov rax, 0x400000003f800000         ; rax = | 2 | 1 |
    movq multipliers, rax               ; multipliers = | 4 | 3 | 2 | 1 |
    mulps sumFreqs, xmm1         ; sumFreqs = | normFreq*4 | normFreq*3 | normFreq*2 | normFreq*1 |

    cycle:
    cmp rdx, 0
    je fin
;    cmp rdx, 4
;    cmp rdx, 2
;    je ?;
    addps phasor, sumFreqs  ; phasor = phasor + |normFreq*4|normFreq*3|normFreq*2|normFreq*1|

    movaps input, [dataBuffIn]
    movaps tmp, input       ; tmp = input
    divps tmp, steps        ; tmp = input/step
    addps tmp, halves       ; tmp = (input/step + 0.5)
    roundps tmp, tmp, 0x01  ; tmp = floor(input/step + 0.5)
    mulps tmp, steps          ; tmp = step*floor(dataBuffEffect[0..3]/step+0.5)

    add dataBuffIn, 16
    add dataBuffOut, 16
    sub framesRead, 4
    jmp cycle
    ; return en rax, o xmm0
    fin:
    pop r15
    pop r14
    pop r13
    pop r12
    add rsp, 4
    pop rbx
    pop rbp
    ret
