#pragma once
#include "types.h"

Result(i64) buf_parse_int(char* buf, usize len, u8 base);
Result(f64) buf_parse_float(char* buf, usize len);
