global wah_wah_index_calc

; vibrato_index_calc(float *bufferIn, unsigned long int framesRead, unsigned long int framesReadTotal, float mod, float depth);
; input enteros
; rdi: *bufferIn
; rsi: bufferLength
; rdx: framesReadTotal
; rcx: minf
; r8: maxf
; r9: samplerate

; input floats
; xmm0: deltas

%define input rdi
%define length rsi
%define framesReadTotal edx
%define minf ecx
%define maxf r8d
%define samplerate r9d
%define deltas xmm0

%define tmpreg r12d

%define indices xmm1
%define sine_args xmm2

; AUXILIARES DE SER NECESARIOS ;
%define tmp xmm11
%define tmp2 xmm12
%define tmp3 xmm13
%define tmp4 xmm14
%define tmp5 xmm15
%define firstTerm xmm13
%define secondTerm xmm14
%define cmpflag xmm15

; REGISTROS CONSTANTES ;
%define ones xmm3               ; float
%define inc_index xmm4          ; float
%define pi xmm5                 ; float
%define two_pi xmm6             ; float
%define Bconstant xmm7          ; float
%define Cconstant xmm8          ; float
%define Pconstant xmm9          ; float
%define triangleWaveSizes xmm10 ; float

_numbers12 dq 0x400000003f800000    ; | 2.0 | 1.0 |
_numbers34 dq 0x4080000040400000    ; | 4.0 | 3.0 |

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
    shufps deltas, deltas, 0x00     ; deltas = |delta|delta|delta|delta|

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
    shufps inc_index, inc_index, 0x00   ; inc_index = |4|4|4|4|

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

    movhps indices, [_numbers34]    ; indices = |4|3|x|x|
    movlps indices, [_numbers12]    ; indices = |4|3|2|1|

    cvtsi2ss tmp, framesReadTotal
    shufps tmp, tmp, 0x00
    addps indices, tmp    ; indices = framesReadTotal+|4.0|3.0|2.0|1.0|
    subps indices, ones   ; indices = |3.0|2.0|1.0|0.0|

    mov tmpreg, maxf        ; tmpreg = maxf
    sub tmpreg, minf        ; tmpreg = maxf-minf
    cvtsi2ss triangleWaveSizes, tmpreg    ; triangleWaveSizes = (float)tmpreg
    divss triangleWaveSizes, deltas       ; triangleWaveSizes = (maxf-minf)/delta
    roundss triangleWaveSizes, triangleWaveSizes, 01b   ; triangleWaveSizes = floor((maxf-minf)/delta)
    addss triangleWaveSizes, ones         ; triangleWaveSizes = floor((maxf-minf)/delta)+1

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
        andps tmp, cmpflag          ; tmp = parityCycle ;me fijo cuáles son pares (queda un 0) y cuáles impares (queda un 1)
        cvtdq2ps tmp, tmp           ; tmp = (float)tmp

        movaps tmp2, triangleWaveSizes  ; tmps = triangleWaveSizes
        movaps cmpflag, indices         ; cmpflag = framesReadTotal+eff_i
        divps cmpflag, tmp2             ; cmpflag = (framesReadTotal+eff_i)/triangleWaveSize
        roundps cmpflag, cmpflag, 01b ; cmpflag = floor(cmpflag) ; cociente entero de la division
        mulps cmpflag, tmp2     ; cmpflag = cociente * divisor
        movaps tmp2, indices    ; tmp2 = framesReadTotal+eff_i
        subps tmp2, cmpflag     ; tmp2 = (framesReadTotal+eff_i)%triangleWaveSize
        addps tmp2, ones        ; tmp2 = thisCycle = (framesReadTotal+eff_i)%triangleWaveSize+1

        movaps tmp3, tmp2       ; tmp3 = thisCycle
        subps tmp3, ones        ; tmp3 = thisCycle-1
        mulps tmp3, deltas      ; tmp3 = (thisCycle-1)*delta
        cvtsi2ss tmp4, minf     ; tmp4 = |x|x|x|(float)minf|
        shufps tmp4, tmp4, 0x00 ; tmp4 = |minf|minf|minf|minf|
        addps tmp3, tmp4        ; tmp3 = (minf+(thisCycle-1)*delta)
        movaps tmp4, ones       ; tmp4 = |1.0|1.0|1.0|1.0|
        subps tmp4, tmp         ; tmp4 = 1.0-parityCycle
        mulps tmp3, tmp4        ; tmp3 = (1-parityCycle)*(minf+(thisCycle-1)*delta)

        movaps sine_args, tmp3  ; sine_args = tmp3

        movaps tmp3, tmp2       ; tmp3 = thisCycle
        mulps tmp3, deltas      ; tmp3 = thisCycle*delta
        cvtsi2ss tmp4, maxf     ; tmp4 = |x|x|x|(float)maxf|
        shufps tmp4, tmp4, 0x00 ; tmp4 = |maxf|maxf|maxf|maxf|
        subps tmp4, tmp3        ; tmp4 = maxf-thisCycle*delta
        mulps tmp4, tmp         ; tmp4 = parityCycle*(maxf-thisCycle*delta)

        addps sine_args, tmp4   ; sine_args = fc_arg = (1-parityCycle)*(minf+(thisCycle-1)*delta)+parityCycle*(maxf-(thisCycle)*delta) ; valor del punto

        mulps sine_args, pi         ; sine_args = M_PI*fc
        cvtsi2ss tmp, samplerate    ; tmp = |x|x|x|(float)samplerate|
        shufps tmp, tmp, 0x00       ; tmp = |samplerate|samplerate|samplerate|samplerate|
        divps sine_args, tmp    ; M_PI*fc/samplerate

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
        movaps tmp, ones            ; tmp = |1.0|1.0|1.0|1.0|
        addps tmp, ones             ; tmp = |2.0|2.0|2.0|2.0|

        mulps firstTerm, tmp        ; fc = 2*sin((M_PI*fc)/inFileStr.samplerate)
        movaps [input], firstTerm

        addps indices, inc_index
        sub length, 4
        add input, 16
        jmp cycle

    ; calculo argumento seno ;
    single_sine_arg:
        cmp length, 0
        je fin

        movss tmp, indices
        divss tmp, triangleWaveSizes
        cvttps2dq tmp, tmp

        movd cmpflag, [_parity]
        andps tmp, cmpflag
        cvtdq2ps tmp, tmp

        movss tmp2, triangleWaveSizes
        movss cmpflag, indices
        divss cmpflag, tmp2
        roundss cmpflag, cmpflag, 01b
        mulss cmpflag, tmp2
        movss tmp2, indices
        subss tmp2, cmpflag
        addss tmp2, ones

        movss tmp3, tmp2
        subss tmp3, ones
        mulss tmp3, deltas
        cvtsi2ss tmp4, minf
        addss tmp3, tmp4
        movss tmp4, ones
        subss tmp4, tmp
        mulss tmp3, tmp4

        movss sine_args, tmp3

        movss tmp3, tmp2
        mulss tmp3, deltas
        cvtsi2ss tmp4, maxf
        subss tmp4, tmp3
        mulss tmp4, tmp

        addss sine_args, tmp4

        mulss sine_args, pi
        cvtsi2ss tmp, samplerate
        divss sine_args, tmp

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
        movss tmp, ones
        addss tmp, ones
        mulss firstTerm, tmp

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
