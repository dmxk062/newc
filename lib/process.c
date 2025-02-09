#include "syscall.h"

void Exit(u8 exitcode) { Syscall_1(Sys_EXIT, (untyped)exitcode); }
