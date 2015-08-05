global sine_asm

; input enteros
; rdi: *input
; rsi: bufferLength

%define input rdi
%define length rsi

%define firstTerm xmm0
%define secondTerm xmm1
%define tmp xmm2
%define Bconstant xmm3
%define Cconstant xmm4
%define Pconstant xmm5

section .text
    sine_asm:
    push rbp        ; convención C
    mov rbp, rsp
    ;sub rsp, N    ; espacio para variables locales
    push rbx
    push r12
    push r13
    push r14
    push r15

    mov eax, 0x3fa2f983 ; 4/pi ~= 1.2732395
    movd Bconstant, eax
    shufps Bconstant, Bconstant, 0x00

    mov eax, 0xbecf817b ; -4/(pi*pi) ~= -0.40528473
    movd Cconstant, eax
    shufps Cconstant, Cconstant, 0x00

    ;mov eax, 0x3e5f3b64 ; 0.218
    mov eax, 0x3e666666 ; 0.225
    movd Pconstant, eax
    shufps Pconstant, Pconstant, 0x00

    cycle:
    cmp length, 4
    jl cycle_single

    movaps firstTerm, [input]

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

    addps firstTerm, secondTerm     ; firstTerm = secondTerm+y
    movaps [input], firstTerm

    sub length, 4
    add input, 16

    jmp cycle

    cycle_single:
    cmp length, 0
    je fin

    movss firstTerm, [input]

    pxor tmp, tmp                   ; tmp = 0
    movss secondTerm, firstTerm    ; secondTerm = input
    subss tmp, secondTerm           ; tmp = -secondTerm
    maxss secondTerm, tmp           ; tmp = abs(input)

    mulss secondTerm, firstTerm     ; secondTerm = input*abs(input)

    mulss firstTerm, Bconstant      ; firstTerm = 4/pi*input
    mulss secondTerm, Cconstant     ; secondTerm = -4/(pi*pi)*input*abs(input)

    addss firstTerm, secondTerm     ; firstTerm = 4/pi*input -4/(pi*pi)*input*abs(input)
    movss [input], firstTerm

    sub length, 1
    add input, 4
    jmp cycle_single

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
