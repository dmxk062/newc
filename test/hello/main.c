#include "str.h"
#include "io.h"
#include "types.h"

u8 start(i32 argc, Str argv[]) {
    if (argc == 1) {
        print(S("Hello, World!\n"));
    } else {
        print(S("Hello, "));
        print(argv[1]);
        print(S("!\n"));
    }

    return 0;
}
