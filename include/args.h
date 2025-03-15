#pragma once
#include <errno.h>
#include <str.h>

#define ARG_NO_PARAM 0x80

enum ArgType : u8 {
    /* Take no parameters */
    ArgT_FLAG = 1 | ARG_NO_PARAM,
    ArgT_COUNT = 2 | ARG_NO_PARAM,

    /* Do take parameters */
    ArgT_INT = 1,
    ArgT_FLOAT = 2,
    ArgT_STRING = 3,
    ArgT_CALLBACK = 4,
    ArgT_INFILE = 5,
    ArgT_OUTFILE = 6,
};

typedef struct {
    void* dest;
    enum ArgType type;

    char short_name;
    Str long_name;

    Str meta_name;
    Str description;
} ArgNamed;

typedef struct {
    void* dest;
    enum ArgType type;

    usize count;
    usize* received;

    Str meta_name;
    bool required;
} ArgPositional;

typedef struct {
    Str program_name;
    Str synopsis;
    Str description;
} ArgProgramDesc;

errno Arguments_parse(ArgProgramDesc* cfg, u32 argc, Str argv[],
                      ArgNamed named[], usize n_named,
                      ArgPositional positional[], usize n_positional);
