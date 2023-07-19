// types.h

#ifndef TYPES

#define TYPES

#ifndef __ASSEMBLER__

#define NULL 0

typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;

typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

typedef unsigned long size_t;
typedef unsigned long ptr_t;
typedef unsigned long uintptr_t;

#define INT32_MAX ((int32)(2147483647))
#define INT32_MIN (-(int32)(2147483647) - 1)
#define UINT32_MAX ((uint32)(4294967295))

#define INT_MAX INT32_MAX
#define INT_MIN INT32_MIN
#define UINT_MAX INT32_MIN

#define bool int
#define true 1
#define false 0

typedef uint64_t timestamp_t;
typedef uint32_t dev_t;

#endif

#endif