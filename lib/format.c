#include "buf.h"
#include "types.h"
#include <errno.h>
#include <float.h>
#include <format.h>
#include <zstr.h>

Result(usize) buf_format_int(char* dst, usize dst_len, u64 val, u8 base,
                             bool sign, u8 zero_fill) {
    static const char digits[256] = "0123456789abcdefghijklmnopqrstuvwxyz";
    const usize bufsize = 64; // 2^64 - 1 in binary

    char digitbuf[bufsize];
    usize index = bufsize - 1;

    bool has_minus = false;
    if (sign) {
        i64 tmp = (i64)val;
        has_minus = tmp < 0;
        if (has_minus)
            val = -tmp;
    }

    if (val == 0) {
        digitbuf[--index] = '0';
    }

    while (val != 0) {
        char digit = digits[val % base];
        digitbuf[--index] = digit;
        val /= base;
    }

    if (has_minus) {
        digitbuf[--index] = '-';
    }

    usize len = bufsize - index - 1;
    if (len < zero_fill) {
        if (zero_fill > dst_len) {
            return Err(usize, LE_BufTooSmall);
        }
        while (len < zero_fill) {
            digitbuf[--index] = '0';
            len++;
        }
    }

    if (len > dst_len) {
        return Err(usize, LE_BufTooSmall);
    }

    buf_copy(dst, digitbuf + index, len);
    return Ok(usize, len);
}

Result(usize) buf_format_float(char* dst, usize dst_len, f64 val,
                               u8 max_decimals, u8 zero_fill) {

    if (val >= F_INFINITY) {
        Str st = S("infinity");
        if (dst_len < st.len) {
            return Err(usize, LE_BufTooSmall);
        }
        buf_copy(dst, st.buf, st.len);
        return Ok(usize, st.len);
    } else if (val <= -F_INFINITY) {
        Str st = S("-infinity");
        if (dst_len < st.len) {
            return Err(usize, LE_BufTooSmall);
        }
        buf_copy(dst, st.buf, st.len);
        return Ok(usize, st.len);
    }

    static const char digits[] = "0123456789";
    bool is_negative = val < 0;
    if (is_negative) {
        val = -val;
    }

    u64 integer = (u64)val;
    f64 fraction = val - (f64)integer;

    const usize buffsize = 64;

    usize iindex = buffsize - 1;
    char ibuffer[buffsize];

    // assemble the fraction buffer from the left
    usize findex = 0;
    char fbuffer[buffsize];

    if (integer == 0) {
        ibuffer[--iindex] = '0';
    }

    while (integer != 0) {
        char digit = digits[integer % 10];
        ibuffer[iindex--] = digit;
        integer /= 10;
    }
    usize ilen = buffsize - iindex - 1;

    if (F_ABS(fraction) < F_EPSILON_HUMAN) {
        fbuffer[findex++] = '0';
    }

    while (fraction > F_EPSILON_HUMAN && findex < buffsize - 1) {
        fraction *= 10;
        u64 digit = (u64)fraction;
        fbuffer[findex++] = digits[digit];
        fraction -= digit;
        if (!--max_decimals) {
            break;
        }
    }
    u64 flen = findex;

    u64 total_len = ilen + flen + 1 + (is_negative ? 1 : 0);
    if (total_len > dst_len) {
        return Err(usize, LE_BufTooSmall);
    }

    if (is_negative) {
        *dst++ = '-';
    }

    buf_copy(dst, ibuffer + iindex + 1, ilen);
    dst[ilen] = '.';
    buf_copy(dst + 1 + ilen, fbuffer, flen);

    return Ok(usize, total_len);
}

static errno format_writeout_buffer(Format_callback fn, void* data,
                                    char* buffer, usize len) {
    while (len > 0) {
        Result(usize) num_written = fn(buffer, len, data);
        if (!num_written.ok) {
            return num_written.err;
        }
        len -= num_written.val;
        buffer += num_written.val;
    }

    return 0;
}

#define FMT_BUFSZ 4096
Result(usize) format_with_callback(Format_callback callback, void* data,
                                   const Str format, FormatArgs values) {
    char buffer[FMT_BUFSZ];

    usize wrptr = 0;
    usize total = 0;
    usize validx = 0;
    usize index = 0;

    u8 cur_base = 10;
    u8 cur_zero_fill = 0;
    u8 cur_max_decimals = 8;
    bool sign = false;

    usize cur_strlen = 0;
    char* cur_strbuf = NULL;

    while (index < format.len) {
        char c = format.buf[index++];
        if (c != '%') {
            buffer[wrptr++] = c;
        } else {
            char code = format.buf[index++];
            switch (code) {
            case '%':
                buffer[wrptr++] = '%';
                break;
            case '.':
                cur_max_decimals = values[validx++]._;
                break;
            case '_': {
                u16 padd = values[validx++]._;
                if (padd <= total + wrptr) {
                    continue;
                }
                u16 diff = padd - (total + wrptr);
                if (diff > FMT_BUFSZ - wrptr) {
                    errno err =
                        format_writeout_buffer(callback, data, buffer, wrptr);
                    if (err) {
                        return Err(usize, err);
                    }
                    total += wrptr;
                    wrptr = 0;
                }

                while (diff-- > 0) {
                    buffer[wrptr++] = ' ';
                }
                break;
            }
            case '0':
                cur_zero_fill = values[validx++]._;
                break;
            case 'c':
                buffer[wrptr++] = values[validx++].c;
                break;
            case 'd':
                cur_base = 10;
                sign = true;
                goto numeric;
            case 'u':
                cur_base = 10;
                sign = false;
                goto numeric;
            case 'x':
                cur_base = 16;
                goto numeric;
            case 'o':
                cur_base = 8;
                goto numeric;
            case 'b':
                cur_base = 2;
                goto numeric;
            numeric: {
                u64 value = values[validx++].i;
                Result(usize) num_written =
                    buf_format_int(buffer + wrptr, FMT_BUFSZ - wrptr, value,
                                   cur_base, sign, cur_zero_fill);
                if (!num_written.ok) {
                    errno err =
                        format_writeout_buffer(callback, data, buffer, wrptr);
                    if (err) {
                        return Err(usize, err);
                    }
                    total += wrptr;
                    wrptr = 0;
                    num_written =
                        buf_format_int(buffer + wrptr, FMT_BUFSZ - wrptr, value,
                                       cur_base, sign, cur_zero_fill);
                }

                cur_base = 10;
                sign = false;
                wrptr += num_written.val;
                break;
            }
            case 'f': {
                f64 value = values[validx++].f;
                Result(usize) num_written =
                    buf_format_float(buffer + wrptr, FMT_BUFSZ - wrptr, value,
                                     cur_max_decimals, cur_zero_fill);
                if (!num_written.ok) {
                    errno err =
                        format_writeout_buffer(callback, data, buffer, wrptr);
                    if (err) {
                        return Err(usize, err);
                    }
                    total += wrptr;
                    wrptr = 0;
                    num_written = buf_format_float(
                        buffer + wrptr, FMT_BUFSZ - wrptr, value,
                        cur_max_decimals, cur_zero_fill);
                }
                wrptr += num_written.val;
                break;
            }
            case 's': {
                Str s = *values[validx++].s;
                cur_strbuf = s.buf;
                cur_strlen = s.len;
                goto stringbuffer;
            }
            case 'z': {
                zstr s = values[validx++].z;
                cur_strbuf = s;
                cur_strlen = zstr_length(s);
                goto stringbuffer;
            }

            stringbuffer: {
                if (cur_strlen <= FMT_BUFSZ - wrptr) {
                    buf_copy(buffer + wrptr, cur_strbuf, cur_strlen);
                    wrptr += cur_strlen;
                } else {
                    errno err =
                        format_writeout_buffer(callback, data, buffer, wrptr);
                    if (err) {
                        return Err(usize, err);
                    }
                    total += wrptr;
                    wrptr = 0;
                    err = format_writeout_buffer(callback, data, cur_strbuf,
                                                 cur_strlen);
                    total += cur_strlen;
                    if (err) {
                        return Err(usize, err);
                    }
                }
                break;
            }
            }
        }

        if (wrptr >= FMT_BUFSZ - 1) {
            errno err = format_writeout_buffer(callback, data, buffer, wrptr);
            total += wrptr;
            wrptr = 0;
            if (err) {
                return Err(usize, err);
            }
        }
    }

    if (wrptr != 0) {
        total += wrptr;
        errno err = format_writeout_buffer(callback, data, buffer, wrptr);
        if (err) {
            return Err(usize, err);
        }
    }

    return Ok(usize, total);
}

static Result(usize) format_buffer_callback(char* buf, usize len, void* data) {
    char** target_buf = *(char***)data;
    usize* target_len = *((usize**)data + 1);

    if (len > *target_len) {
        return Err(usize, LE_BufTooSmall);
    }

    buf_copy(*target_buf, buf, len);
    *target_buf += len;
    *target_len -= len;

    return Ok(usize, len);
}

Result(usize)
    buf_format(char* buf, usize len, const Str format, FormatArgs values) {
    u64 args[2] = {(u64)&buf, (u64)&len};
    return format_with_callback(format_buffer_callback, &args, format, values);
}

Result(usize) Str_format_into(Str* str, Str format, FormatArgs values) {
    Result(usize) num_formatted =
        buf_format(str->buf, str->size, format, values);
    if (num_formatted.ok) {
        str->len = num_formatted.val;
    }
    return num_formatted;
}
