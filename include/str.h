#pragma once

typedef struct String {
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
} String;
Result_define_with_ptr(String);

#define S(_strlit) ((String){.size = 0, .len = sizeof(_strlit), .buf = _strlit})
#define S_as_rbuf(_str) _str.buf, _str.len
#define S_as_wbuf(_str) _str.buf, _str.size
