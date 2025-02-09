#include "io.h"
#include "str.h"
#include "syscall.h"
#include "types.h"

Result(u32) fd_open(const zstr path, enum OpenFlag flags, u32 mode) {
    i32 res = (i32)Syscall_3(Sys_OPEN, (untyped)path, (untyped)flags, (untyped)mode);

    return_SysRes(u32, res);
}

Result(usize) fd_write(u32 fd, const void* const buf, usize size) {
    ssize res = (ssize)Syscall_3(Sys_WRITE, (untyped)fd, (untyped)buf, (untyped)size);

    return_SysRes(usize, res);
}

Result(usize) fd_read(u32 fd, void* buf, usize size) {
    ssize res = (ssize)Syscall_3(Sys_READ, (untyped)fd, (untyped)buf, (untyped)size);

    return_SysRes(usize, res);
}

void print(Str str) {
    fd_write(IO_Stdout, S_as_rbuf(str));
}
