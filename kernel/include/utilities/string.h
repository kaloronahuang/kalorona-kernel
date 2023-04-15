// string.h
#ifndef UTILITIES_STRING

#define UTILITIES_STRING

#include <types.h>
#include <parameters.h>

void *memset(void *dst, int val, size_t siz);

void *memcpy(void *dst, const void *src, size_t siz);

void *memmove(void *dst, const void *src, size_t siz);

int memcmp(const void *lhs, const void *rhs, size_t siz);

void *memchr(const void *ptr, int ch, size_t siz);

size_t strlen(const char *s);

size_t strnlen(const char *str, size_t strsz);

char *strchr(const char *str, int ch);

char *strrchr(const char *str, int ch);

char *strcpy(char *dst, const char *src);

char *strncpy(char *dst, const char *src, size_t siz);

int strcmp(const char *lhs, const char *rhs);

int strncmp(const char *lhs, const char *rhs, size_t siz);

ulong strtoul(const char *str, char **endptr, uint base);

#endif