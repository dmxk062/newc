#include "zstr.h"

usize zstr_length(zstr str) {
    usize count = 0;
    while (*str++) {
        count++;
    }

    return count;
}
