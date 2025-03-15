#include "args.h"
#include "buf.h"
#include "format.h"
#include "io.h"
#include "parse.h"
#include "private/args.h"
#include "str.h"
#include "types.h"

static enum ArgResult arg_handler_int(Str value, void* dest, u64 count,
                                      Str* error_msg) {
    u8 base = 10;
    char* buf = value.buf;
    usize len = value.len;
    if (value.len > 2) {
        if (value.buf[0] == '0')
            switch (value.buf[1]) {
            case 'x': {
                base = 16;
                break;
            }
            case 'o': {
                base = 8;
                break;
            }
            case 'b': {
                base = 2;
                break;
            }
            }
        if (base != 10) {
            buf += 2;
            len -= 2;
        }
    }

    Result(i64) integer_val = buf_parse_int(buf, len, base);
    if (!integer_val.ok) {
        Str_format_into(error_msg, S("Invalid integer: %s\n"),
                        F({.s = &value}));
        return ArgResultError;
    }

    ((i64*)dest)[count] = integer_val.val;
    return ArgResultOk;
}

static enum ArgResult arg_handler_float(Str value, void* dest, u64 count,
                                        Str* error_msg) {
    Result(f64) float_val = buf_parse_float(value.buf, value.len);
    if (!float_val.ok) {
        Str_format_into(error_msg, S("Invalid decimal number: %s\n"),
                        F({.s = &value}));
        return ArgResultError;
    }

    ((f64*)dest)[count] = float_val.val;
    return ArgResultOk;
}

static enum ArgResult arg_handler_string(Str value, void* dest, u64 count,
                                         Str* error_msg) {
    Str* strings = dest;
    strings[count].buf = value.buf;
    strings[count].len = value.len;
    return ArgResultOk;
}

static enum ArgResult arg_handler_set(Str value, void* dest, u64 count,
                                      Str* error_msg) {
    *(bool*)dest = true;
    return ArgResultOk;
}
static enum ArgResult arg_handler_counter(Str value, void* dest, u64 count,
                                          Str* error_msg) {
    (*(i64*)dest)++;
    return ArgResultOk;
}

static ArgHandler handlers[] = {
    [ArgT_FLAG] = arg_handler_set,      [ArgT_COUNT] = arg_handler_counter,

    [ArgT_INT] = arg_handler_int,       [ArgT_FLOAT] = arg_handler_float,
    [ArgT_STRING] = arg_handler_string,
};

void print_help(ArgProgramDesc* desc, ArgNamed* named, usize n_named,
                ArgPositional* positional, usize n_positional) {
    fd_fwrite_continuous(IO_Stderr, S("Usage: %s [OPTION]..."),
                         F({.s = &desc->program_name}));
    for (usize i = 0; i < n_positional; i++) {
        ArgPositional arg = positional[i];
        Str fmt = arg.count > 1 ? (arg.required ? S(" %s...") : S(" [%s]..."))
                                : (arg.required ? S(" %s") : S(" [%s]"));
        fd_fwrite_continuous(IO_Stderr, fmt,
                             F({.s = &positional[i].meta_name}));
    }
    fd_fwrite_continuous(
        IO_Stderr,
        S("\n%s\n\nOptions:\n  -h, --help                            "
          "Show this message\n"),
        F({.s = &desc->synopsis}));
    for (usize i = 0; i < n_named; i++) {
        ArgNamed arg = named[i];
        if (arg.short_name) {
            fd_fwrite_continuous(
                IO_Stderr, S("  -%c, --%s%c%s%_%s\n"),
                F({.c = arg.short_name}, {.s = &arg.long_name},
                  {.c = ((arg.type & ARG_NO_PARAM) ? ' ' : '=')},
                  {.s = &arg.meta_name}, {._ = 40}, {.s = &arg.description}));
        }
    }
    fd_fwrite_continuous(IO_Stderr, S("\n%s\n"), F({.s = &desc->description}));
}

static ArgNamed* find_named_arg(ArgNamed* args, usize len, char short_name,
                                Str long_name) {
    for (usize i = 0; i < len; i++) {
        ArgNamed arg = args[i];
        if (short_name) {
            if (arg.short_name && arg.short_name == short_name) {
                return args + i;
            }
        } else if (Str_eq(long_name, arg.long_name)) {
            return args + i;
        }
    }

    return NULL;
}

errno Arguments_parse(ArgProgramDesc* cfg, u32 argc, Str argv[],
                      ArgNamed named[], usize n_named,
                      ArgPositional positional[], usize n_positional) {

    Str* error_buffer = Str_new(1024);

    bool done_parsing_named = false;
    usize positional_idx = 0;

    for (usize index = 1; index < argc; index++) {
        Str arg = argv[index];
        if (!done_parsing_named && Str_eq(arg, S("--"))) {
            done_parsing_named = true;
            continue;
        }
        if (!done_parsing_named && Str_startswith(arg, S("--"))) {
            Result(usize) eq_sign_pos = buf_find_byte(S_as_rbuf(arg), '=');
            Str arg_name =
                (Str){.buf = arg.buf + 2,
                      .len = (eq_sign_pos.ok ? eq_sign_pos.val : arg.len) - 2};

            ArgNamed* spec = find_named_arg(named, n_named, 0, arg_name);
            if (!spec) {
                if (Str_eq(arg_name, S("help"))) {
                    goto show_help;
                }

                fd_fwrite_continuous(IO_Stderr,
                                     S("Unrecognized long option: `--%s`\n"),
                                     F({.s = &arg_name}));
                goto abort_parsing;
            }

            enum ArgType type = spec->type;
            Str value;
            if (!(type & ARG_NO_PARAM)) {
                if (eq_sign_pos.ok) {
                    value = (Str){.buf = arg.buf + eq_sign_pos.val + 1,
                                  .len = arg.len - eq_sign_pos.val - 1};
                } else {
                    index++;
                    if (index == argc) {
                        fd_fwrite_continuous(
                            IO_Stderr,
                            S("Missing mandatory argument `%s` for "
                              "long option: `--%s`\n"),
                            F({.s = &spec->meta_name}, {.s = &arg_name}));
                        goto abort_parsing;
                    }
                    value = argv[index];
                }
            } else {
                value = (Str){};
            }

            enum ArgResult res =
                handlers[spec->type](value, spec->dest, 0, error_buffer);
            if (res == ArgResultError) {
                goto show_error;
            }
        } else if (!done_parsing_named && arg.buf[0] == '-' && arg.len > 1) {
            for (usize ai = 1; ai < arg.len; ai++) {
                char flag = arg.buf[ai];
                ArgNamed* spec = find_named_arg(named, n_named, flag, (Str){});
                if (!spec) {
                    if (flag == 'h') {
                        goto show_help;
                    }

                    fd_fwrite_continuous(
                        IO_Stderr, S("Unrecognized short option: `-%c`\n"),
                        F({.c = flag}));
                    goto abort_parsing;
                }

                enum ArgType type = spec->type;
                Str value;
                if (!(type & ARG_NO_PARAM)) {
                    if (ai < arg.len - 1) {
                        value = (Str){.buf = arg.buf + ai + 1,
                                      .len = arg.len - ai - 1};
                    } else {
                        index++;
                        if (index == argc) {
                            fd_fwrite_continuous(
                                IO_Stderr,
                                S("Missing mandatory argument `%s` for "
                                  "short option: `-%c`\n"),
                                F({.s = &spec->meta_name}, {.c = flag}));
                            goto abort_parsing;
                        }
                        value = argv[index];
                    }
                    ai = arg.len;
                } else {
                    value = (Str){};
                }
                enum ArgResult res =
                    handlers[spec->type](value, spec->dest, 0, error_buffer);
                if (res == ArgResultError) {
                    goto show_error;
                }
            }
        } else {
            if (positional_idx < n_positional) {
                ArgPositional desc = positional[positional_idx];
                enum ArgResult res = handlers[desc.type](
                    arg, desc.dest, *desc.received, error_buffer);
                if (res == ArgResultError) {
                    goto show_error;
                } else if (desc.count > *desc.received + 1 && desc.count > 1) {
                    (*desc.received)++;
                } else {
                    (*desc.received)++;
                    positional_idx++;
                }
            }
        }
    }

    for (usize i = 0; i <= positional_idx; i++) {
        ArgPositional arg = positional[positional_idx];
        if (arg.required && !*arg.received) {
            fd_fwrite_continuous(IO_Stderr, S("Missing required argument: %s\n"), F({.s = &arg.meta_name}));
            goto abort_parsing;
        }
    }

    return 0;

show_error: {
    fd_write(IO_Stderr, error_buffer->buf, error_buffer->len);
    goto abort_parsing;
}

show_help:
    print_help(cfg, named, n_named, positional, n_positional);
    goto abort_parsing;

abort_parsing:
    Str_free(error_buffer);
    return 1;
}
