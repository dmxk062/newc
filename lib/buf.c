#include <buf.h>

void buf_copy(void* dst, const void* src, usize size) {
    u64* td = (u64*)dst;
    u64* ts = (u64*)src;
    u8 *cd, *cs;

    // copy 8 bytes at once
    for (; size > 8; size -= 8) {
        *td++ = *ts++;
    }

    // we have less than 1 u64 left, copy u8s now
    cd = (u8*)td;
    cs = (u8*)ts;

    while (size-- > 0) {
        *cd++ = *cs++;
    }
}

void buf_zero(void* dst, usize len) {
    u64* dst64 = (u64*)dst;
    while (len >= 8) {
        *dst64++ = 0;
        len -= 8;
    }

    u8* dest8 = (u8*)dst64;
    while (len > 0) {
        *dest8++ = 0;
        len--;
    }
}

bool buf_eq(void* b1, usize b1_len, void* b2, usize b2_len) {
    if (b1_len != b2_len)
        return false;

    u64* u1 = (u64*)b1;
    u64* u2 = (u64*)b2;

    for (; b1_len > 8; b1_len -= 8) {
        if (*u1++ != *u2++)
            return false;
    }

    u8* c1 = (u8*)u1;
    u8* c2 = (u8*)u2;

    while (b1_len-- > 0) {
        if (*c1++ != *c2++)
            return false;
    }

    return true;
}
