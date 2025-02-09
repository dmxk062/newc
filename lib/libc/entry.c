#include "alloc.h"
#include "str.h"
#include "process.h"
#include "private/libc.h"

struct __newc_libc_state __newc_global_state = {NULL};

extern u8 start(i32, Str[]);

void __newc_libc_start(i32 argc, char** argv) {
    __newc_global_state.initial_argv = argv;
    __newc_global_state.initial_envv = argv + argc + 1;

    Str* new_argv = alloc(argc * sizeof(Str));
    for (i32 i=0; i < argc; i++) {
        new_argv[i] = Str_view_zstr(argv[i]);
    }
    __newc_global_state.argv = new_argv;

    u8 res = start(argc, new_argv);
    Exit(res);
}
