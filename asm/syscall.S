# translate between C and SysV kernel calling conventionsl
# for Syscall_6, the last argument is on the stack
.global Syscall_0, Syscall_1, Syscall_2, Syscall_3, Syscall_5, Syscall_5, Syscall_6

Syscall_0:
    mov     %rdi,   %rax

    syscall
    ret

Syscall_1:
    mov     %rdi,   %rax
    mov     %rsi,   %rdi

    syscall
    ret

Syscall_2:
    mov     %rdi,   %rax
    mov     %rsi,   %rdi
    mov     %rdx,   %rsi

    syscall
    ret

Syscall_3:
    mov     %rdi,   %rax
    mov     %rsi,   %rdi
    mov     %rdx,   %rsi
    mov     %rcx,   %rdx

    syscall
    ret

Syscall_4:
    mov     %rdi,   %rax
    mov     %rsi,   %rdi
    mov     %rdx,   %rsi
    mov     %rcx,   %rdx
    mov     %r8,    %r10

    syscall
    ret

Syscall_5:
    mov     %rdi,   %rax
    mov     %rsi,   %rdi
    mov     %rdx,   %rsi
    mov     %rcx,   %rdx
    mov     %r8,    %r10
    mov     %r9,    %r8

    syscall
    ret

Syscall_6:
    mov     %rdi,   %rax
    mov     %rsi,   %rdi
    mov     %rdx,   %rsi
    mov     %rcx,   %rdx
    mov     %r8,    %r10
    mov     %r9,    %r8
    mov     8(%rsp),%r9

    syscall
    ret
