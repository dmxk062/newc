#include "buf.h"
#include "io.h"
#include "zstr.h"
#include <str.h>
#include <alloc.h>

Str Str_view_zstr(zstr c_str) {
    usize len = zstr_length(c_str);

    return (Str){.size = 0, .len = len, .buf = c_str};
}

Str* Str_new(usize size) {
    usize alloc_size = sizeof(Str) + size;

    Str* new = talloc(alloc_size);
    if (!new) {
        return NULL;
    }

    new->buf = (char*)new + sizeof(Str);
    new->len = 0;
    new->size = size;

    return new;
}

void Str_free(Str* str) {
    free(str - sizeof(Str));
}

bool Str_eq(Str s1, Str s2) {
    return buf_eq(s1.buf, s1.len, s2.buf, s2.len);
}

bool Str_startswith(Str s, Str prefix) {
    if (prefix.len > s.len) {
        return false;
    }

    return buf_eq(s.buf, prefix.len, prefix.buf, prefix.len);
}

