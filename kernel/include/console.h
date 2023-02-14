// console.h
#include <types.h>

#ifndef CONSOLE

#define CONSOLE

void print_char(char c);

int print_str(char *s);

int print_uint16(uint16 x);

int print_uint16_by_base(uint16 x, uint16 base);

int print_uint32(uint32 x);

int print_uint32_by_base(uint32 x, uint32 base);

int print_uint64(uint64 x);

int print_uint64_by_base(uint64 x, uint64 base);

int print_int16(int16 x);

int print_int16_by_base(int16 x, int16 base);

int print_int32(int32 x);

int print_int32_by_base(int32 x, int32 base);

int print_int64(int64 x);

int print_int64_by_base(int64 x, int64 base);

#endif