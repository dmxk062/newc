#include "str.h"
#include <env.h>
#include <io.h>

u8 start(i32 argc, Str argv[], Env* env) {
    Result(Str) new_PATH = Env_get(env, S("PATH"));
    if (!new_PATH.ok) {
        print(S("PATH is unset\n"));
        return 1;
    }
    fprint(S("PATH=%s\n"), F({.s = &new_PATH.val}));
    Env_unset(env, S("PATH"));
    Result(Str) changed_PATH = Env_get(env, S("PATH"));
    if (!changed_PATH.ok) {
        print(S("PATH is unset\n"));
        return 1;
    }
    fprint(S("PATH=%s\n"), F({.s = &changed_PATH.val}));
    return 0;
}
