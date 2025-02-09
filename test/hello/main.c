#include "str.h"
#include "io.h"
#include "types.h"

static Str message = S("Hello World!\n");

i32 main(i32 argc, zstr argv[]) {

    print(message);
}
