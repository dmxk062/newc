#pragma once
#include <types.h>

enum Syscall {
    Sys_READ = 0,
    Sys_WRITE = 1,
    Sys_OPEN = 2,
    Sys_CLOSE = 3,

    Sys_STAT = 4,
    Sys_FSTAT = 5,
    Sys_LSTAT = 6,
    Sys_POLL = 7,
    Sys_SEEK = 8,
    Sys_MMAP = 9,

    Sys_MUNMAP = 11,

    Sys_SIGACTION = 13,
    Sys_SIGPROCMASC = 14,
    Sys_SIGRETURN = 15,

    Sys_IOCTL = 16,

    Sys_ACCESS = 21,
    Sys_PIPE = 22,

    Sys_DUP = 32,
    Sys_DUP2 = 33,

    Sys_PAUSE = 34,
    Sys_NANOSLEEP = 35,

    Sys_GETPID = 39,

    Sys_SOCKET = 41,
    Sys_CONNECT = 42,
    Sys_ACCEPT = 43,
    Sys_SENDTO = 44,
    Sys_RECVFROM = 45,
    Sys_SENDMSG = 46,
    Sys_RECVMSG = 47,
    Sys_SHUTDOWN = 48,
    Sys_BIND = 49,
    Sys_LISTEN = 50,

    Sys_FORK = 57,
    Sys_VFORK = 58,

    Sys_EXECVE = 59,
    Sys_EXIT = 60,
    Sys_KILL = 62,

    Sys_FCNTL = 72,

    Sys_GETCWD = 79,
    Sys_CHDIR = 80,

    Sys_LINK = 86,
    Sys_UNLINK = 87,

    Sys_GETTIMEOFDAY = 96,

    Sys_GETDENTS = 78,
    Sys_GETDENTS64 = 217,

    Sys_GETUID = 102,
    Sys_GETGID = 104,
    Sys_SETUID = 105,
    Sys_SETGID = 106,

    Sys_GETEUID = 107,
    Sys_GETEGID = 108,

    Sys_PRCTL = 157,

    Sys_CLOCK_GETTIME = 228,
};

untyped Syscall_0(enum Syscall call);
untyped Syscall_1(enum Syscall call, untyped arg1);
untyped Syscall_2(enum Syscall call, untyped arg1, untyped arg2);
untyped Syscall_3(enum Syscall call, untyped arg1, untyped arg2, untyped arg3);
untyped Syscall_4(enum Syscall call, untyped arg1, untyped arg2, untyped arg3,
                  untyped arg4);
untyped Syscall_5(enum Syscall call, untyped arg1, untyped arg2, untyped arg3,
                  untyped arg4, untyped arg5);
untyped Syscall_6(enum Syscall call, untyped arg1, untyped arg2, untyped arg3,
                  untyped arg4, untyped arg5, untyped arg6);
