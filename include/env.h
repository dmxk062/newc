#pragma once
#include "str.h"

#define ENV_PAGE_SIZE (1024 * sizeof(zstr*)) // ~1k strings should be enough for the pointers in environ

typedef struct Env {
    zstr* envv;
    usize len;
    usize size;
} Env;


Env* Env_new();
Env* Env_copy(Env* env);
Env* Env_new_from_envvec(zstr* envv);
void Env_free(Env* env);

Result(usize) Env_find(Env* env, Str name);
Result(zstr) Env_get_zstr(Env* env, Str name);
Result(Str) Env_get(Env* env, Str name);

errno Env_set(Env* env, Str name, Str value);
errno Env_unset(Env* env, Str name);
