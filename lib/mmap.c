#include "mmap.h"
#include "types.h"
#include <syscall.h>

Result(void_p) mmap(untyped addr, u64 len, enum MapProt prot, enum MapFlag flags, u64 fd, u64 offset) {
    i64 ret = (i64)Syscall_6(Sys_MMAP, 
            addr,
            (untyped)len,
            (untyped)prot,
            (untyped)flags,
            (untyped)fd,
            (untyped)offset);
    if (ret < 0) 
        return Err(void_p, -ret);
    else
        return Ok(void_p, (untyped)ret);
}


errno mmunmap(untyped addr, u64 len) {
    return (errno) -(i64)Syscall_2(Sys_MUNMAP,
            addr,
            (untyped)len);
}
