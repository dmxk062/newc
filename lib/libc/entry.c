#include "syscall.h"
#include "process.h"
#include "private/libc.h"

struct __newc_libc_state __newc_global_state = {NULL};

extern i32 main(i32, zstr[]);

void __newc_libc_start(i32 argc, char** argv) {
    __newc_global_state.initial_argv = argv;
    __newc_global_state.initial_envv = argv + argc + 1;

    i32 res = main(argc, argv);
    Exit(res);
}
