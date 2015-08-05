global flanger_index_calc

 ;flanger_index_calc(unsigned int *bufferIn, unsigned long int framesRead, unsigned long int framesReadTotal, float rate, unsigned int delayInFrames, unsigned int maxDelayInFrames)
; input enteros
; rdi: *input
; rsi: bufferLength
; rdx: framesReadTotal
; rcx: delayInFrames
; r8: maxDelayInFrames
; xmm0: rate

%define input rdi
%define length rsi
%define framesReadTotal edx
%define delayInFrames ecx
%define maxDelayInFrames r8d

%define rates xmm0
%define firstTerm xmm1
%define tmp1 xmm1
%define secondTerm xmm2
%define tmp2 xmm2
%define tmp xmm3
%define Bconstant xmm4
%define Cconstant xmm5
%define Pconstant xmm6
%define pi xmm7
%define two_pi xmm8
%define ones xmm9
%define indices xmm10
%define inc_index xmm11
%define sine_args xmm12
%define cmpflag xmm13
%define delays xmm14
%define max_delays xmm15

_numbers12 dq 0x400000003f800000    ; | 2 | 1 |
_numbers34 dq 0x4080000040400000    ; | 4 | 3 |

section .text
    flanger_index_calc:
    push rbp        ; convención C
    mov rbp, rsp
    ;sub rsp, N    ; espacio para variables locales
    push rbx
    push r12
    push r13
    push r14
    push r15

    ; SETUP ;
    shufps rates, rates, 0x00   ; rates = |rate|rate|rate|rate|

    movhps indices, [_numbers34]    ; indices = | 4 | 3 | x | x |
    movlps indices, [_numbers12]    ; indices = | 4 | 3 | 1 | 2 |

    movd tmp, framesReadTotal
    shufps tmp, tmp, 0x00
    cvtdq2ps tmp, tmp     ; (float)framesReadTotal
    addps indices, tmp    ; indices = framesReadTotal+|4|3|2|1|

    movd delays, delayInFrames
    shufps delays, delays, 0x00     ; delays = |delayInFrames|delayInFrames|delayInFrames|delayInFrames|
    cvtdq2ps delays, delays         ; (float)delays

    movd max_delays, maxDelayInFrames
    shufps max_delays, max_delays, 0x00 ; max_delays = |maxDelayInFrames|maxDelayInFrames|maxDelayInFrames|maxDelayInFrames|

    mov eax, 0x40490fdb ; 3.1415927
    movd pi, eax
    shufps pi, pi, 0x00

    mov eax, 0x40c90fdb ; 2*pi
    movd two_pi, eax
    shufps two_pi, two_pi, 0x00

    mov eax, 0x3f800000 ; 1.0
    movd ones, eax
    shufps ones, ones, 0x00   ; ones = |1.0|1.0|1.0|1.0|
    cvtps2dq ones, ones       ; ones = (int) 1.0 = 1

    mov eax, 0x40800000
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

    cycle:
        cmp length, 4
        jl single_cycle_setup

    ; CALCULO ARGUMENTOS DEL SENO ;
    cycle_sine_args:
        movaps sine_args, indices ; sine_args = (framesReadTotal+(eff_i+1))
        mulps sine_args, rates    ; sine_args = sine_args*(rate/inFileStr.samplerate)
        mulps sine_args, two_pi   ; sine_args = sine_args*2*pi

        arg_to_interval:            ; llevo los argumentos del seno al intervalo (0,2*pi)
            movaps cmpflag, two_pi          ; cmpflag = |two_pi|two_pi|two_pi|two_pi|
            cmpps cmpflag, sine_args, 0x01  ; cmpflag = two_pi < sine_args
            ptest cmpflag, cmpflag
            jz arg_to_interval_cont         ; si es cero, es porque no hay ningún argumento mayor que 2*pi

            movaps tmp, sine_args
            divps tmp, two_pi
            roundps tmp, tmp, 01b; me quedo con el cociente de la división
            mulps tmp, two_pi       ; multiplico por el divisor
            subps sine_args, tmp    ; ahora tengo todo entre (0, 2pi)

        arg_to_interval_cont:       ; llevo lo que está entre (pi, 2pi) a (-pi, 0)
            movaps cmpflag, pi      ; cmpflag = |pi|pi|pi|pi|
            cmpps cmpflag, sine_args, 0x01  ; cmpflag = pi < sine_args
            ptest cmpflag, cmpflag
            jz calc_sine

            andps cmpflag, two_pi       ; me quedo con 2*pi en los lugares donde pi < sine_args
            subps sine_args, cmpflag    ; ahora está todo entre (-pi, pi)

    ; CALCULO SENO ;
    calc_sine:
        movaps firstTerm, sine_args

        pxor tmp, tmp                   ; tmp = 0
        movaps secondTerm, firstTerm    ; secondTerm = input
        subps tmp, secondTerm           ; tmp = -secondTerm
        maxps secondTerm, tmp           ; tmp = abs(input)

        mulps secondTerm, firstTerm     ; secondTerm = input*abs(input)

        mulps firstTerm, Bconstant      ; firstTerm = 4/pi*input
        mulps secondTerm, Cconstant     ; secondTerm = -4/(pi*pi)*input*abs(input)

        addps firstTerm, secondTerm     ; firstTerm = 'y' = 4/pi*input -4/(pi*pi)*input*abs(input)

        ; correccion
        movaps secondTerm, firstTerm    ; secondTerm = y

        pxor tmp, tmp                   ; tmp = 0
        subps tmp, secondTerm           ; tmp = -y
        maxps secondTerm, tmp           ; secondTerm = abs(y)

        mulps secondTerm, firstTerm     ; secondTerm = abs(y)*y
        subps secondTerm, firstTerm     ; secondTerm = abs(y)*y-y
        mulps secondTerm, Pconstant     ; secondTerm = P*(abs(y)*y-y)

        addps firstTerm, secondTerm     ; firstTerm = secondTerm+y = sine(sine_args)

    ; CALCULO INDICE ;
    calc_index:
        pxor tmp, tmp               ; tmp = 0
        subps tmp, firstTerm        ; tmp = - sine(sine_args)
        maxps firstTerm, tmp        ; firstTerm = abs(sine(sine_args))

        mulps firstTerm, delays     ; firstTerm = abs(sine(sine_args))*delayInFrames
        roundps firstTerm, firstTerm,  10b ; firstTerm = ceil(abs(sine(sine_args))*delayInFrames)

        movaps tmp, indices         ; tmp = indices
        subps tmp, firstTerm        ; tmp = framesReadTotal+eff_i-ceil(abs(sine(sine_args))*delayInFrames)
        cvtps2dq tmp, tmp           ; tmp = (int) tmp

    mod_maxdelay:      ; llevo los indices al intervalo [0,maxDelayInFrames) calculando tmp%maxDelayInFrames
        movaps cmpflag, max_delays  ; cmpflag = |maxDelayInFrames|maxDelayInFrames|maxDelayInFrames|maxDelayInFrames|
        cmpps cmpflag, tmp, 0x01    ; cmpflag = maxDelayInFrames < (framesReadTotal+eff_i-ceil(abs(sine(sine_args))*delayInFrames))
        ptest cmpflag, cmpflag
        jz continue_cycle                ; si es cero, no hace falta reducir el argumento

        movaps tmp1, tmp            ; tmp1 = tmp
        divps tmp1, max_delays      ; tmp1 = tmp / maxDelayInFrames
        roundps tmp1, tmp1, 01b     ; me quedo con la parte entera de la division
        mulps tmp1, max_delays      ; cociente*divisor
        subps tmp, tmp1             ; en tmp me queda el resto de la división, tmp % maxDelayInFrames

    continue_cycle:
        subps tmp, ones            ; corrijo un offset de 1 que hay (indices es framesReadTotal+eff_i+1, y en la cuenta anterior a mod_maxdelay necesito sacar ese 1)
        pxor firstTerm, firstTerm       ; firstTerm = |0|0|0|0|
        maxps tmp, firstTerm            ; saco los negativos de tmp (sólo pasa en el primer índice)
        movaps [input], tmp

        addps indices, inc_index        ; aumento los indices
        sub length, 4
        add input, 16

        jmp cycle

    single_cycle_setup:
        shufps indices, indices, 0xFF ; pongo el indice más grande en todo el registro
    single_cycle:
        cmp length, 0
        je fin

        movss sine_args, indices
        mulss sine_args, rates
        mulss sine_args, two_pi

    ; CALCULO ARGUMENTO SENO ;
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
        movss secondTerm, firstTerm    ; secondTerm = input
        subss tmp, secondTerm           ; tmp = -secondTerm
        maxss secondTerm, tmp           ; tmp = abs(input)

        mulss secondTerm, firstTerm     ; secondTerm = input*abs(input)

        mulss firstTerm, Bconstant      ; firstTerm = 4/pi*input
        mulss secondTerm, Cconstant     ; secondTerm = -4/(pi*pi)*input*abs(input)

        addss firstTerm, secondTerm     ; firstTerm = 4/pi*input -4/(pi*pi)*input*abs(input)

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
        pxor tmp, tmp
        subss tmp, firstTerm
        maxss firstTerm, tmp

        mulss firstTerm, delays
        roundss firstTerm, firstTerm, 10b

        movss tmp, indices
        subss tmp, firstTerm
        cvtps2dq tmp, tmp

    single_mod_maxdelay:
        movss cmpflag, max_delays
        cmpss cmpflag, tmp, 0x01
        ptest cmpflag, cmpflag

        jz continue_single_cycle

        movss tmp1, tmp
        divss tmp1, max_delays
        roundss tmp1, tmp1, 01b
        mulss tmp1, max_delays
        subss tmp, tmp1

    continue_single_cycle:
        subss tmp, ones
        pxor firstTerm, firstTerm
        maxss tmp, firstTerm
        movss [input], tmp

        addss indices, ones

        sub length, 1
        add input, 4
        jmp single_cycle

    fin:
        pop r15
        pop r14
        pop r13
        pop r12
        ;add rsp, N
        pop rbx
        pop rbp
        ret
