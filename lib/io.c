#include "io.h"
#include "str.h"
#include "syscall.h"
#include "types.h"

Result(u32) fd_open(const zstr path, enum OpenFlag flags, u32 mode) {
    i32 res =
        (i32)Syscall_3(Sys_OPEN, (untyped)path, (untyped)flags, (untyped)mode);

    return_SysRes(u32, res);
}

Result(usize) fd_write(u32 fd, const void* const buf, usize size) {
    ssize res =
        (ssize)Syscall_3(Sys_WRITE, (untyped)fd, (untyped)buf, (untyped)size);

    return_SysRes(usize, res);
}

Result(usize) fd_read(u32 fd, void* buf, usize size) {
    ssize res =
        (ssize)Syscall_3(Sys_READ, (untyped)fd, (untyped)buf, (untyped)size);

    return_SysRes(usize, res);
}


void print(Str str) { fd_write(IO_Stdout, S_as_rbuf(str)); }


static Result(usize) format_write_callback(char* buffer, usize len, void* data) {
    u32 fd = *(u32*)data;

    usize total_written = 0;

    while (total_written < len) {
        Result(usize) num_written = fd_write(fd, buffer + total_written, len - total_written);
        if (!num_written.ok) {
            return num_written;
        }
        total_written += num_written.val;
    }

    return Ok(usize, total_written);
}

Result(usize) fd_fwrite_continuous(u32 fd, Str format, Format_value* values) {
    return format_with_callback(format_write_callback, &fd, format, values);
}
