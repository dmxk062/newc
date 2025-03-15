#include "buf.h"
#include "types.h"
#include <errno.h>
#include <parse.h>

static inline i64 char_to_int(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'a' && c <= 'z') {
        return c - 'a' + 10;
    } else if (c >= 'A' && c <= 'Z') {
        return c - 'A' + 10;
    }

    return -1;
}

Result(i64) buf_parse_int(char* buf, usize len, u8 base) {
    if (len == 0) {
        return Err(i64, LE_BufTooShort);
    }

    i64 result = 0;

    bool is_negative = buf[0] == '-';
    if (is_negative) {
        buf++;
        len--;
    }

    for (usize i = 0; i < len; i++) {
        i64 digit = char_to_int(buf[i]);
        if (digit < 0) {
            return Err(i64, LE_NotNumeric);
        } else if (digit >= base) {
            return Err(i64, LE_InvalidBase);
        }

        i64 pwr = 1;
        for (i64 j = 0; j < len - 1 - i; j++) {
            pwr *= base;
        }
        result += digit * pwr;
    }

    if (is_negative) result = -result;

    return Ok(i64, result);
}

Result(f64) buf_parse_float(char* buf, usize len) {
    if (len == 0) {
        return Err(f64, LE_BufTooShort);
    }

    u64 target = len;
    Result(usize) decimal_pos = buf_find_byte(buf, len, '.');
    if (decimal_pos.ok) {
        target = decimal_pos.val;
    }

    Result(i64) integer_part = buf_parse_int(buf, target, 10);
    if (!integer_part.ok) {
        return Err(f64, integer_part.err);
    }

    if (target == len) {
        return Ok(f64, integer_part.val);
    }

    f64 fraction = 0.0;
    for (usize i = decimal_pos.val + 1; i < len; i++) {
        i64 digit = char_to_int(buf[i]);
        if (digit < 0) {
            return Err(f64, LE_NotNumeric);
        } else if (digit >= 10) {
            return Err(f64, LE_InvalidBase);
        }
        f64 pwr = 0.1;
        for (usize j = decimal_pos.val + 1; j < i; j++) {
            pwr /= 10.0;
        }
        fraction += digit * pwr;
    }

    f64 result = integer_part.val + fraction;
    if (integer_part.val < 0) result = -result;

    return Ok(f64, result);
}
