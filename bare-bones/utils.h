#include <stdint.h>
#include <stddef.h>

void sleep(uint32_t ticks) {
    for (uint32_t i = 0; i < ticks; i++) {
        for (uint32_t j = 0; j < 10000; j++) {
            __asm__ __volatile__("nop");
        }
    }
}

size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

void iota(uint32_t* arr, uint32_t length) {
	for (uint32_t i = 0; i < length; i++) arr[i] = i;
}
