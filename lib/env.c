#include "buf.h"
#include "errno.h"
#include "types.h"
#include "zstr.h"
#include <alloc.h>
#include <env.h>

Env* Env_new() {
    Env* environ = talloc(ENV_PAGE_SIZE);
    if (!environ) {
        return NULL;
    }

    environ->envv = (zstr*)((char*)environ + sizeof(Env));
    environ->len = 0;
    environ->size = ENV_PAGE_SIZE - sizeof(Env);

    return environ;
}

Env* Env_new_from_envvec(zstr* envv) {
    Env* env = Env_new();
    if (!env) {
        return NULL;
    }

    usize count = 0;
    zstr* envp = envv;
    while (*envp++ != NULL) {
        count++;
    }

    buf_copy(env->envv, envv, count * sizeof(zstr*));
    env->len = count;

    return env;
}

Env* Env_copy(Env* envv) {
    Env* new_env = Env_new();
    if (!new_env) {
        return NULL;
    }

    buf_copy(new_env->envv, envv->envv, envv->len);
    new_env->len = envv->len;

    return new_env;
}

Result(usize) Env_find(Env* env, Str name) {
    for (usize i = 0; i < env->len; i++) {
        u64 len = zstr_length(env->envv[i]);
        if (len < name.len + 2)
            continue;

        if (buf_eq(name.buf, name.len, env->envv[i], name.len) &&
            env->envv[i][name.len] == '=') {
            return Ok(usize, i);
        }
    }

    return Err(usize, 0);
}

Result(zstr) Env_get_zstr(Env* env, Str name) {
    Result(usize) entry = Env_find(env, name);
    if (!entry.ok) {
        return Err(zstr, LE_KeyNotFound);
    }

    return Ok(zstr, env->envv[entry.val] + name.len + 1);
}

Result(Str) Env_get(Env* env, Str name) {
    Result(zstr) entry = Env_get_zstr(env, name);
    if (!entry.ok) {
        return Err(Str, entry.err);
    }

    return Ok(Str, Str_view_zstr(entry.val));
}

errno Env_set(Env* env, Str name, Str value) {
    Result(usize) index = Env_find(env, name);
    zstr* target = NULL;

    if (index.ok) {
        target = &env->envv[index.val];
        free(env->envv[index.val]);
    } else {
        if (env->len == env->size) {
            return SE_OutOfMem;
        }
        target = &env->envv[env->len];
        env->len++;
        env->envv[env->len] = NULL;
    }

    zstr buffer = talloc(name.len + value.len + 2 * sizeof(char));
    if (!buffer) {
        return SE_OutOfMem;
    }

    buf_copy(buffer, name.buf, name.len);
    buffer[name.len] = '=';
    buf_copy(&buffer[name.len + sizeof(char)], value.buf, value.len);
    buffer[name.len + value.len + sizeof(char)] = '\0';
    *target = buffer;

    return 0;
}

errno Env_unset(Env* env, Str name) {
    Result(usize) index = Env_find(env, name);
    if (!index.ok) {
        return LE_KeyNotFound;
    }

    free(env->envv[index.val]);

    // shift it upwards
    for (usize i = index.val; i < env->len; i++) {
        env->envv[i] = env->envv[i + 1];
    }
    env->len--;

    return 0;
}
