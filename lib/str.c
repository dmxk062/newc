#include "zstr.h"
#include <str.h>

Str Str_view_zstr(zstr c_str) {
    usize len = zstr_length(c_str);

    return (Str){.size = 0, .len = len, .buf = c_str};
}
