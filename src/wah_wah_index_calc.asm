global wah_wah_index_calc

; vibrato_index_calc(float *bufferIn, unsigned long int framesRead, unsigned long int framesReadTotal, float mod, float depth);
; input enteros
; rdi: *bufferIn
; rsi: bufferLength
; rdx: framesReadTotal
; rcx: minf
; r8: maxf
; r9: *fc
; [rbp+16]: samplerate
; input floats
; xmm0: deltas

%define input rdi
%define length rsi
%define framesReadTotal edx
%define minf ecx
%define maxf r8d
%define deltas xmm0
%define fc [r9d]
%define samplerate [rbp+16]

%define tmpreg r12d

%define fcs xmm1
%define delays xmm1
%define indices xmm2
%define sine_args xmm6

; AUXILIARES DE SER NECESARIOS ;
%define tmp xmm3
%define tmp2 xmm0
%define firstTerm xmm10
%define secondTerm xmm11
%define cmpflag xmm9

; REGISTROS CONSTANTES ;
%define triangleWaveSizes xmm15
%define ones xmm4
%define inc_index xmm5
%define pi xmm7
%define two_pi xmm8
%define Bconstant xmm12
%define Cconstant xmm13
%define Pconstant xmm14

_numbers12 dq 0x0000000200000001    ; | 2 | 1 |
_numbers34 dq 0x0000000400000003    ; | 4 | 3 |

_parity dq 0x0000000100000001       ; | 1 | 1 |

section .text
    wah_wah_index_calc:
    push rbp        ; convención C
    mov rbp, rsp
    ;sub rsp, N    ; espacio para variables locales
    push rbx
    push r12
    push r13
    push r14
    push r15

    ; SETUP ;
    shufps delays, delays, 0x00         ; mods = |mod|mod|mod|mod|
    shufps fcs, fcs, 0x00     ; depths = |depth|depth|depth|depth|

    movhps indices, [_numbers34]    ; indices = |4.0|3.0|x|x|
    movlps indices, [_numbers12]    ; indices = |4.0|3.0|2.0|1.0|

    movd tmp, framesReadTotal
    shufps tmp, tmp, 0x00
    addps indices, tmp    ; indices = framesReadTotal+|4|3|2|1|

    mov eax, 0x40490fdb ; 3.1415927
    movd pi, eax
    shufps pi, pi, 0x00

    mov eax, 0x40c90fdb ; 2*pi
    movd two_pi, eax
    shufps two_pi, two_pi, 0x00

    mov eax, 0x3f800000 ; 1.0
    movd ones, eax
    shufps ones, ones, 0x00   ; ones = |1.0|1.0|1.0|1.0|

    mov eax, 0x40800000 ; 4.0
    movd inc_index, eax
    shufps inc_index, inc_index, 0x00   ; inc_index = |4.0|4.0|4.0|4.0|

    mov eax, 0x3fa2f983 ; 4/pi ~= 1.2732395
    movd Bconstant, eax
    shufps Bconstant, Bconstant, 0x00

    mov eax, 0xbecf817b ; -4/(pi*pi) ~= -0.40528473
    movd Cconstant, eax
    shufps Cconstant, Cconstant, 0x00

    mov eax, 0x3e5f3b64 ; 0.218
;    mov eax, 0x3e666666 ; 0.225
    movd Pconstant, eax
    shufps Pconstant, Pconstant, 0x00

    mov tmpreg, maxf        ; tmpreg = maxf
    sub tmpreg, minf        ; tmpreg = maxf-minf
    cvtsi2ss tmp, tmpreg    ; tmp = (float)tmpreg
    divss tmp, deltas       ; tmp = (maxf-minf)/delta
    roundss tmp, tmp, 01b   ; tmp = floor((maxf-minf)/delta)
    addss tmp, ones         ; tmp = floor((maxf-minf)/delta)+1
    ;cvtss2si tmpreg, tmp    ; tmp = (int) tmp = triangleWaveSize

    movss triangleWaveSizes, tmp
    shufps triangleWaveSizes, triangleWaveSizes, 0x00 ; triangleWaveSizes = |triangleWaveSize|triangleWaveSize|triangleWaveSize|triangleWaveSize|

    cycle:
        cmp length, 4
        jl single_sine_arg

    ; CALCULO ARGUMENTOS DEL SENO ;
    cycle_sine_args:
        movaps tmp, indices         ; tmp = framesReadTotal+eff_i
        divps tmp, triangleWaveSizes ; tmp = (framesReadTotal+eff_i)/triangleWaveSize
        cvttps2dq tmp, tmp          ; tmp = (int)floor(tmp) ; es decir, queda division entera

        movlps cmpflag, [_parity]   ; |x|x|1|1|
        movhps cmpflag, [_parity]   ; |1|1|1|1|
        andps cmpflag, tmp          ; me fijo cuáles son pares (queda un 0) y cuáles impares (queda un 1)


        mulps sine_args, two_pi     ; sine_args = 2*pi*(framesReadTotal+eff_i+1)
        mulps sine_args, mods       ; sine_args = (mod*2*pi*(framesReadTotal+eff_i+1))

        args_to_interval:   ; llevo los argumentos del seno al intervalo (-pi,pi)
            movaps cmpflag, two_pi          ; cmpflag = |two_pi|two_pi|two_pi|two_pi|
            cmpps cmpflag, sine_args, 0x01  ; cmpflag = two_pi < sine_args
            ptest cmpflag, cmpflag
            jz args_to_interval_cont         ; si es cero, están todos entre (0, 2*pi)

            movaps tmp, sine_args       ; tmp = sine_args
            divps tmp, two_pi           ; tmp = sine_args/(2*pi)
            roundps tmp, tmp, 01b       ; tmp = floor(sine_args/(2*pi)) = cociente entero de la division
            mulps tmp, two_pi           ; tmp = cociente * divisor
            subps sine_args, tmp        ; sine_args = floor(sine_args/(2*i))*(2*pi) = sine_args % (2*pi)
                                        ; tengo todo en el rango (0, 2*pi)

        args_to_interval_cont:              ; llevo lo que está entre (pi, 2*pi) a (-pi, pi)
            movaps cmpflag, pi              ; cmpflag = |pi|pi|pi|pi|
            cmpps cmpflag, sine_args, 0x01  ; cmpflag = pi < sine_args
            ptest cmpflag, cmpflag
            jz calc_sine                    ; si es cero, ya estan todos los argumentos entre (-pi, pi)

            andps cmpflag, two_pi           ; cmpflag = 2*pi en los lugares donde pi < sine_args
            subps sine_args, cmpflag        ; ahora todo esta entre (-pi, pi)
    calc_sine:
        movaps firstTerm, sine_args

        pxor tmp, tmp                   ; tmp = 0
        movaps secondTerm, firstTerm    ; secondTerm = sine_args
        subps tmp, secondTerm           ; tmp = -secondTerm
        maxps secondTerm, tmp           ; tmp = abs(sine_args)

        mulps secondTerm, firstTerm     ; secondTerm = sine_args*abs(sine_args)
        mulps firstTerm, Bconstant      ; firstTerm = 4/pi*input
        mulps secondTerm, Cconstant     ; secondTerm = -4/(pi*pi)*sine_args*abs(sine_args)

        addps firstTerm, secondTerm     ; firstTerm = 'y' = 4/pi*sine_args-4/(pi*pi)*sine_args*abs(sine_args)

        ; correccion ;
        movaps secondTerm, firstTerm    ; secondTerm = y

        pxor tmp, tmp                   ; tmp = 0
        subps tmp, secondTerm           ; tmp = -y
        maxps secondTerm, tmp           ; secondTerm = abs(y)

        mulps secondTerm, firstTerm     ; secondTerm = abs(y)*y
        subps secondTerm, firstTerm     ; secondTerm = abs(y)*y-y
        mulps secondTerm, Pconstant     ; secondTerm = P*(abs(y)*y-y)

        addps firstTerm, secondTerm     ; firstTerm = secondTerm+y = sine(sine_args) = current_mod

    ; CALCULO INDICE ;
    calc_index:
        mulps firstTerm, depths         ; firstTerm = current_mod*depth
        addps firstTerm, delays         ; firstTerm = delay+current_mod*depth
        addps firstTerm, ones           ; firstTerm = 1.0 + delay + depth * current_mod = tap

        movaps [input], firstTerm

        addps indices, inc_index
        sub length, 4
        add input, 16
        jmp cycle

    ; calculo argumento seno ;
    single_sine_arg:
        cmp length, 0
        je fin

        movss sine_args, indices
        mulss sine_args, two_pi
        mulss sine_args, mods

        single_arg_to_interval:
            movss cmpflag, two_pi
            cmpss cmpflag, sine_args, 0x01
            ptest cmpflag, cmpflag
            jz single_arg_to_interval_cont

            movss tmp, sine_args
            divss tmp, two_pi
            roundss tmp, tmp, 01b
            mulss tmp, two_pi
            subss sine_args, tmp

        single_arg_to_interval_cont:
            movss cmpflag, pi
            cmpss cmpflag, sine_args, 0x01
            ptest cmpflag, cmpflag
            jz single_calc_sine

            andps cmpflag, two_pi
            subss sine_args, cmpflag

        ; CALCULO SENO ;
        single_calc_sine:
            movss firstTerm, sine_args

            pxor tmp, tmp                   ; tmp = 0
            movss secondTerm, firstTerm     ; secondTerm = sine_arg
            subss tmp, secondTerm           ; tmp = -secondTerm
            maxss secondTerm, tmp           ; tmp = abs(sine_arg)

            mulss secondTerm, firstTerm     ; secondTerm = sine_arg*abs(sine_arg)

            mulss firstTerm, Bconstant      ; firstTerm = 4/pi*sine_arg
            mulss secondTerm, Cconstant     ; secondTerm = -4/(pi*pi)*input*abs(sine_arg)

            addss firstTerm, secondTerm     ; firstTerm = 4/pi*sine_arg -4/(pi*pi)*sine_arg*abs(sine_arg)

            ; correccion
            movss secondTerm, firstTerm

            pxor tmp, tmp
            subss tmp, secondTerm
            maxss secondTerm, tmp

            mulss secondTerm, firstTerm
            subss secondTerm, firstTerm
            mulss secondTerm, Pconstant

            addss firstTerm, secondTerm

    ; CALCULO INDICE ;
    single_calc_index:
        mulss firstTerm, depths     ; firstTerm = current_mod*depth
        addss firstTerm, delays     ; firstTerm = delay+current_mod*depth
        addss firstTerm, ones       ; firstTerm = 1.0+delay+depth*current_mod = tap

        movss [input], firstTerm

        addss indices, ones
        sub length, 1
        add input, 4
        jmp single_sine_arg

    fin:
    pop r15
    pop r14
    pop r13
    pop r12
    ;add rsp, N
    pop rbx
    pop rbp
    ret
