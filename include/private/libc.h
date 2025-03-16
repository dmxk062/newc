#pragma once
#include "env.h"
#include "str.h"

struct __newc_libc_state {
    zstr const* initial_argv;
    zstr const* initial_envv;

    Str* argv;
    Env* envv;
};

extern struct __newc_libc_state __newc_global_state;
