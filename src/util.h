#pragma once

#include "raylib.h"
#include <cstdint>
#include <string>

#define PPM 32

typedef float    f32;
typedef double   f64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

u8* B_CreateFileBuffer(const char* path, i32* dataSize);
Image B_LoadImage(std::string path); 