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

void itoa(uint32_t num, char* buffer, int buffer_size) {
    if (buffer_size < 2) return;
    
    int i = 0;
    if (num == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return;
    }

    char temp[32];
    int temp_index = 0;
    
    while (num > 0 && temp_index < 31) {
        temp[temp_index++] = '0' + (num % 10);
        num /= 10;
    }
    
    while (temp_index > 0 && i < buffer_size - 1) {
        buffer[i++] = temp[--temp_index];
    }
    
    buffer[i] = '\0';
}
