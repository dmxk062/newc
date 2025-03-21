#pragma once

typedef struct Str {
    /*
     * Size of the byte array used to store the string
     * if 0, string is read only or a view
     *  - freeing or mutating is UB
     */
    u32 size;

    /*
     * Amount of bytes part of the string's readable value
     * does not include the null-separator for zero terminated strings
     */
    u32 len;

    /*
     * Buffer storing the string's content
     */
    char* buf;
} Str;
Result_define_with_ptr(Str);

Str Str_view_zstr(zstr c_str);

Str* Str_new(usize size);
void Str_free(Str* str);

bool Str_eq(Str s1, Str s2);
bool Str_startswith(Str s, Str prefix);

#define S(_strlit) ((Str){.size = 0, .len = sizeof(_strlit) - 1, .buf = _strlit})
#define S_as_rbuf(_str) _str.buf, _str.len
#define S_as_wbuf(_str) _str.buf, _str.size

