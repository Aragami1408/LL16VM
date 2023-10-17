#ifndef BASE_TYPES_H
#define BASE_TYPES_H

// NOTE(higanbana): Basic Types
#include <stdint.h>
#include "base_defines.h"

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;


#if OS_MAC == 1
	#include <stddef.h>
#endif

typedef uintptr_t usize;
typedef intptr_t isize;

typedef i8 b8;
typedef i16 b16;
typedef i32 b32;
typedef i64 b64;

typedef float f32;
typedef double f64;

#endif
