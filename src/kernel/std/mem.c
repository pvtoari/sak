#include <kernel/std/mem.h>

void *kmemcpy(void *dest, const void *src, size_t n) {
    uint8_t *d = dest;
    const uint8_t *s = src;

    for (size_t i = 0; i < n; i++) d[i] = s[i];

    return dest;
}

void *kmemset(void *dest, int c, size_t n) {
    for (char *p = dest; p != dest + n; ++p) *p = c;
}

void *kmalloc(size_t size) {
    size = align_up(size);
    size_t total_size = size + sizeof(size_t);

    if (used + total_size > HEAP_SIZE) return NULL;

    size_t *size_ptr = (size_t *) (heap + used);
    *size_ptr = size;

    void *ptr = (void *) (size_ptr + 1);
    used += total_size;

    return ptr;
}

void *krealloc(void *ptr, size_t size) {
    if (!ptr) return kmalloc(size);

    size_t *size_ptr = (size_t *) ptr - 1;
    size_t old_size = *size_ptr;

    if (size <= old_size && align_up(size) >= old_size - ALIGNMENT) return ptr;

    void *new_ptr = kmalloc(size);
    if (!new_ptr) return NULL;

    size_t copy_size = old_size < size ? old_size : size;
    kmemcpy(new_ptr, ptr, copy_size);

    return new_ptr;
}

void kfree(void *ptr) {
    (void) ptr;
    return;
}