#pragma once

typedef void* untyped;

/*
 * Integer types
 */
typedef signed long int i64;
typedef unsigned long int u64;

typedef signed int i32;
typedef unsigned int u32;

typedef signed short i16;
typedef unsigned short u16;

typedef signed char i8;
typedef unsigned char u8;

typedef u64 usize;
typedef i64 ssize;

/*
 * Positive values communicate function specific erros
 * Negative values are the same as system errors
 */
typedef i16 errno;

typedef void* untyped;

/*
 * Floating point
 */

typedef long double f128;
typedef double f64;
typedef float f32;

/*
 * Booleans
 */
typedef _Bool bool;
#define true ((bool)1)
#define false ((bool)0)

/*
 * Types communicating different intent
 */

// zero terminated c-style string
typedef char* zstr;

/*
 * Result types, passed as a structure
 * Use _p as a suffix for pointers
 * e.g. Result(i64), Result(i64_p)
 */
// clang-format off
#define Result_define(type, name)                                              \
    typedef struct __result_struct_##name {                                    \
        bool ok;                                                               \
        union {                                                                \
            errno err;                                                         \
            type val;                                                          \
        };                                                                     \
    } __result_t_##name

#define Result(name) __result_t_##name
#define Ok(name, ...) ((Result(name)){.ok = true, .val = (__VA_ARGS__)})
#define Err(name, ...) ((Result(name)){.ok = false, .err = (__VA_ARGS__)})
#define return_SysRes(name, ...) return ((Result(name)){.ok = ((__VA_ARGS__) < 0), .val = (__VA_ARGS__)})

#define Result_define_with_ptr(type)                                           \
    Result_define(type, type);                                                 \
    Result_define(type*, type##_p)
// clang-format on
/*
 * Built in types
 */

Result_define_with_ptr(i64);
Result_define_with_ptr(u64);
Result_define_with_ptr(i32);
Result_define_with_ptr(u32);
Result_define_with_ptr(i16);
Result_define_with_ptr(u16);
Result_define_with_ptr(i8);
Result_define_with_ptr(u8);

Result_define_with_ptr(usize);
Result_define_with_ptr(ssize);

Result_define_with_ptr(f128);
Result_define_with_ptr(f64);
Result_define_with_ptr(f32);

Result_define_with_ptr(zstr);
Result_define_with_ptr(char);

Result_define_with_ptr(bool);
Result_define(void*, void_p);

/*
 * Syntax-ish macros
 */

#define Array_length(arr) (sizeof(arr) / sizeof(arr[0]))

/*
 * Constants
 */
#define NULL ((untyped)0)
