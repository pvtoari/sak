#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "vga.h"
#include "vector.h"
#include "utils.h"
#include "pepe.h"

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

#define VGA_WIDTH	80
#define VGA_HEIGHT  25
#define VGA_MEMORY  0xB8000 

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer = (uint16_t*)VGA_MEMORY;

void terminal_initialize(void) 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_scroll() {
    for (int y = 1; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            terminal_buffer[(y - 1) * VGA_WIDTH + x] = terminal_buffer[y * VGA_WIDTH + x];
        }
    }
    
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
    }
    
    terminal_row = VGA_HEIGHT - 1;
    terminal_column = 0;
}

void terminal_putchar(char c) 
{
	switch(c) {
		case '\n':
			terminal_column = 0;
			terminal_row++;
			break;
		case '\r':
			terminal_column = 0;
			break;
		default:
			terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
			if (++terminal_column == VGA_WIDTH) {
				terminal_column = 0;
				terminal_row++;
			}
			break;
	}

	if (terminal_row == VGA_HEIGHT) terminal_scroll();
}


void terminal_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) 
{
	terminal_write(data, strlen(data));
}

void terminal_clear(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_putentryat(' ', terminal_color, x, y);
        }
    }
    
    terminal_row = 0;
    terminal_column = 0;
}

void show_vector_status(vector* v, const char* name) {
    char buffer[64];
    
    terminal_writestring("Vector '");
    terminal_writestring(name);
    terminal_writestring("': ");
    
    itoa(v->size(v), buffer, 10);
    terminal_writestring("Size=");
    terminal_writestring(buffer);
    
    terminal_writestring(", Cap=");
    itoa(v->vectorList.capacity, buffer, 10);
    terminal_writestring(buffer);
    
    terminal_writestring(", Items=[");
    for(int i = 0; i < v->size(v); i++) {
        if(i > 0) terminal_writestring(", ");
        itoa((uint32_t)v->get(v, i), buffer, 10);
        terminal_writestring(buffer);
    }
    
    terminal_writestring("]");
    terminal_putchar('\n');
}

void vector_test() {
	uint32_t sleep_amount = 100000;
    vector v;
    vector_init(&v);
    
    terminal_writestring("\nStarting vector tests...\n\n");
    sleep(sleep_amount);
    
    terminal_writestring("Adding 10 elements...\n");
    for(uint32_t i = 0; i < 10; i++) {
        v.add(&v, (void*)(i + 1));
        show_vector_status(&v, "test");
        sleep(sleep_amount);
    }

    terminal_writestring("\nModifying element at index 5 -> 99...\n");
    sleep(sleep_amount);
    v.set(&v, 5, (void*) 99);
    show_vector_status(&v, "test");
    sleep(sleep_amount);
    
    terminal_writestring("\nErasing 5 elements...\n");
    sleep(sleep_amount);
    for(int i = 0; i < 5; i++) {
        v.delete(&v, 0);
        show_vector_status(&v, "test");
        sleep(sleep_amount);
    }
    
    terminal_writestring("\nResizing to size 20...\n");
    sleep(sleep_amount);
    v.resize(&v, 20);
    show_vector_status(&v, "test");
    sleep(sleep_amount);
    
    terminal_writestring("\nCompleted tests!\n");
}

void kernel_main(void) {
	terminal_initialize();

	vector_test();
	sleep(10000000);
	
	uint32_t length = 10;
	uint32_t arr[length];
	iota(arr, length);
	
	int i = 0;
	while(true) {
		terminal_clear();
		terminal_setcolor(i+1);
		terminal_writestring(frames[i]);
		i = (i + 1) % FRAMES_COUNT;
		sleep(8000);
	}
}
