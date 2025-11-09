#ifndef STRING_H
#define STRING_H

#include <stddef.h>

size_t strlen(const char *str);

int strncmp(const char *s1, const char *s2, size_t n);

int strcmp(const char *s1, const char *s2);

char *convert(unsigned int num, int base);

#endif // STRING_H