#pragma once
#include <str.h>
#include <types.h>

typedef union {
    u64 i;  // d, u, x, o, b
    f64 f;  // f
    zstr z; // z
    Str* s; // s
    char c; // c
    u16 _; // %0, %.
} Format_value;

typedef Format_value FormatArgs[];

Result(usize) buf_format_int(char* buf, usize buf_len, u64 val, u8 base, bool sign, u8 zero_fill);
Result(usize) buf_format_float(char* buf, usize buf_len, f64 val, u8 max_decimals, u8 zero_fill);

Result(usize) buf_format(char* buf, usize buf_len, const Str format, FormatArgs values);

typedef Result(usize) (*Format_callback)(char* buf, usize len, void* data);
Result(usize) format_with_callback(Format_callback callback, void* data, const Str format, FormatArgs values);

#define F(...) ((FormatArgs){__VA_ARGS__})
