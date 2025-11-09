#ifndef MISC_H
#define MISC_H

#include <stdint.h>
#include <stddef.h>

void sleep(uint32_t ticks);

void iota(uint32_t *arr, uint32_t length);

void itoa(uint32_t num, char *buffer, int buffer_size);

#endif // MISC_H
