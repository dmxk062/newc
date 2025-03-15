#include "io.h"
#include "format.h"
#include "str.h"

u8 start(i32 argc, Str argv[]) {
    char buffer[256];
    
    Result(usize) n_formatted = buf_format(buffer, sizeof(buffer), S("Hello: %s, %d\n"), F({.s = &argv[0]}, {.i = 10}));
    fd_write(IO_Stdout, buffer, n_formatted.val);

    return 0;
}
