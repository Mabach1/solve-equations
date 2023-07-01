#ifndef __DEFINES_H__
#define __DEFINES_H__

#include <stdio.h>
#include <stdlib.h>

/* types */
typedef unsigned char                  u8;
typedef unsigned short                u16;
typedef unsigned int                  u32;
typedef unsigned long long            u64;
                                    
typedef signed char                    s8;
typedef signed short                  s16;
typedef signed int                    s32;
typedef signed long long              s64;
                                    
typedef char                           i8;
typedef short                         i16;
typedef int                           i32;
typedef long long                     i64;
                                    
typedef float                         f32;
typedef double                        f64;
typedef long double                  f128;

typedef ssize_t                     isize;
typedef size_t                      usize;

/* special types */
typedef u8                          byte;

/* bounds of respective data types */
#define U8_MAX              0xFF
#define U16_MAX             0xFFFF
#define U32_MAX             0xFFFFFFFF
#define U64_MAX             0xFFFFFFFFFFFFFFFFULL

#define S8_MAX              0x7F
#define S8_MIN              (-S8_MAX - 1)
#define S16_MAX             0x7FFF
#define S16_MIN             (-S16_MAX - 1)
#define S32_MAX             0x7FFFFFFF
#define S32_MIN             (-S32_MAX - 1)
#define S64_MAX             0x7FFFFFFFFFFFFFFFLL
#define S64_MIN             (-S64_MAX - 1)

#define I8_MAX              S8_MAX
#define I8_MIN              S8_MIN
#define I16_MAX             S16_MAX
#define I16_MIN             S16_MIN
#define I32_MAX             S32_MAX
#define I32_MIN             S32_MIN
#define I64_MAX             S64_MAX
#define I64_MIN             S64_MIN

#define F32_MAX             3.402823466e+38F
#define F32_MIN             (-F32_MAX)
#define F64_MAX             1.7976931348623157e+308
#define F64_MIN             (-F64_MAX)
#define F128_MAX            1.18973149535723176502e+4932L
#define F128_MIN            (-F128_MAX)

#define USIZE_MAX           0xFFFFFFFFFFFFFFFFULL
#define ISIZE_MAX           S64_MAX
#define ISIZE_MIN           S64_MIN

/* boolean */
typedef enum { false, true } bool;

/* memory size */
#define Bytes(x)     ((u64) x)
#define KiloBytes(x) ((u64) x * 1024) 
#define MegaBytes(x) ((u64) x * 1024 * 1024) 
#define GigaBytes(x) ((u64) x * 1024 * 1024 * 1024)

#define table_len(table) sizeof (table) / sizeof (*table)
#define LOG(msg) fprintf(stderr, "[%s]:[%llu] [%s]\n", __FILE__, __LINE__, msg);
#define LOG_VAL(__format, ...)                                              \
    do {                                                                    \
        char __buffer[8092];                                                \
        snprintf(__buffer, sizeof(__buffer), __format, __VA_ARGS__);        \
        LOG(__buffer)                                                       \
    } while (0)

#define ASSERT(_e, ...) if (!(_e)) { fprintf(stderr,  __VA_ARGS__); exit(1); }

#endif
