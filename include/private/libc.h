#pragma once

struct __newc_libc_state {
    zstr const* initial_argv;
    zstr const* initial_envv;
};

extern struct __newc_libc_state __newc_global_state;
