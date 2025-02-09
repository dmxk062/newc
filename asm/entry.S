# main program entry point
.global _start

.extern __newc_lib_start

.section .text

_start:
    xor     %rbp,       %rbp

    # argc
    pop     %rdi
    # argv
    mov     %rsp,       %rsi
    and     $0xFFFFFFFFFFFFFFF0,    %rsp
    # this function will exit for us
    call    __newc_libc_start
