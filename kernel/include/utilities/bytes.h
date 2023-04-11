// bytes.h
#include <types.h>

#ifndef UTILITIES_BYTES

#define UTILITIES_BYTES

void flip_bytes_inplace(void *x, size_t numOfBytes);

uint16 flip_bytes_16(uint16 x);
uint32 flip_bytes_32(uint32 x);
uint64 flip_bytes_64(uint64 x);

int test_and_set_bit(uint8 *map, ulong index);
int test_and_clear_bit(uint8 *map, ulong index);
int test_bit(uint8 *map, ulong index);

#endif