global copy_asm

; input
; rdi: *buffer_in;
; rsi: *buffer_out;
; rdx: framesRead

%define dataBuffIn rdi
%define dataBuffOut rsi
%define length rdx

%define input xmm0

section .text
    copy_asm:
    push rbp        ; convención C
    mov rbp, rsp
    push rbx
    push r12
    push r13
    push r14
    push r15

    cycle:
        cmp length,0   ; ya recorrí todo el buffer?
        je fin
        cmp length,4  ; caso borde: cantidad de frames impar
        jle odd_frames

        movaps input, [dataBuffIn]
        movaps [dataBuffOut], input

        add dataBuffIn,16
        add dataBuffOut,16

        sub length, 4  ; recorrí 4 frames
        jmp cycle

    odd_frames:
        movss input, [dataBuffIn]
        movss [dataBuffOut], input

        add dataBuffIn, 4
        add dataBuffOut, 4
        sub length, 1

        cmp length, 0
        jne odd_frames

    fin:
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret
