#ifndef STRING_H
#define STRING_H

#include <stddef.h>

size_t strlen(const char *str) {
	size_t len = 0;
	while (str[len]) len++;

	return len;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    if (n == 0) return 0;
    
    while (n-- && *s1 && *s2 && (*s1 == *s2)) {
        s1++;
        s2++;
    }

    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int strcmp(const char *s1, const char *s2) {
    while(*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

char *convert(unsigned int num, int base)
{
    static char representation[] = "0123456789ABCDEF";
    static char buffer[50];
    char *ptr;

    ptr = &buffer[49];
    *ptr = '\0';

    do {
        *--ptr = representation[num % base];
        num /= base;
    } while(num != 0);

    return (ptr);
}

#endif // STRING_H