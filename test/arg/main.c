#include <args.h>
#include <io.h>
#include <types.h>

bool average = false;
ArgNamed named[] = {
    {&average, ArgT_FLAG, 'a', S("average"), S(""), S("Average the values")},
};

#define MAX_NUMS 16384
f64 numbers[MAX_NUMS] = {};
usize received_numbers = 0;

ArgPositional positional[] = {
    {numbers, ArgT_FLOAT, MAX_NUMS, &received_numbers, S("NUMBER"), true},
};

ArgProgramDesc desc = {
    .program_name = S("sum"),
    .synopsis = S("Sum all of its arguments (and demonstrate argument parsing "
                  "in the process)"),
    .description =
        S("Sumarizes all of its arguments"),
};

u8 start(u32 argc, Str argv[]) {
    errno err = Arguments_parse(&desc, argc, argv, named, Array_length(named),
                                positional, Array_length(positional));

    if (err) {
        return err;
    }


    f64 sum = 0;
    for (usize i = 0; i < received_numbers; i++) {
        sum += numbers[i];
    }

    if (average) {
        sum /= received_numbers;
    }

    fprint(S("%f\n"), F({.f = sum}));
    return 0;
}
