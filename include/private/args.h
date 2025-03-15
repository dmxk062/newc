#pragma once
#include "str.h"

enum ArgResult {
    ArgResultError,
    ArgResultOk,
    ArgResultKeep,
};

typedef enum ArgResult (*ArgHandler)(Str value, void* dest, u64 count, Str* error_msg);
