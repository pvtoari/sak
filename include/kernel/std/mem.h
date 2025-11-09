#ifndef MEM_H
#define MEM_H

#include <stddef.h>
#include <stdint.h>

#define HEAP_SIZE 0x100000  // 1 MB
//#define HEAP_SIZE 0x1000000  // 2 GB
#define ALIGNMENT 8

extern unsigned char _end;

static unsigned char* heap = &_end;
static size_t used = 0;

void *kmemcpy(void *dest, const void *src, size_t n);

void *kmemset(void *dest, int c, size_t n);

static inline size_t align_up(size_t size) {
    return (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
}

void *kmalloc(size_t size);

void *krealloc(void *ptr, size_t size);

void kfree(void *ptr);

#endif // MEM_H
