#pragma once
#include "format.h"
#include "types.h"
#include "str.h"

enum OpenFlag { 
    Open_RONLY = 0,
    Open_WONLY = 1,
    Open_RW = 2,
    Open_CREATE = 64,
    Open_EXCLUSIVE = 128,
    Open_APPEND = 1024
};

enum IO_StdStreams {
    IO_Stdin = 0,
    IO_Stdout = 1,
    IO_Stderr = 2,
};

Result(u32) fd_open(const zstr path, enum OpenFlag flags, u32 mode);

Result(usize) fd_write(u32 fd, const void* const buf, usize size);
Result(usize) fd_read(u32 fd, void* buf, usize size);
void print(Str str);

Result(usize) fd_fwrite_continuous(u32 fd, Str format, FormatArgs values);
void fprint(Str format, FormatArgs values);
