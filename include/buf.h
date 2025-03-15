#pragma once
#include "types.h"

void buf_copy(void* dst, const void* src, usize size);
void buf_zero(void* dst, usize len);

bool buf_eq(void* b1, usize b1_len, void* b2, usize b2_len);

Result(usize) buf_find_byte(char* buf, usize len, char byte);
