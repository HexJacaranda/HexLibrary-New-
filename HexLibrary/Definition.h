#pragma once
#include <stddef.h>
#include <wchar.h>
#include <type_traits>
#if _MSC_VER>=1600
#define _USE_RIGHT_VALUE_REFERENCE 1//”“÷µ“˝”√
#endif
#define FALSE 0

#ifndef _WIN64
#define _LIB_X64 false
#else
#define _LIB_X64 true
#endif

#if _LIB_X64
typedef long long offset_t;
#else
typedef long offset_t;
#endif

typedef char Byte;

struct void_t {

};