#ifndef VGA_H
#define VGA_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#include "std/string.h"
#include "std/misc.h"
#include "math/math.h"
#include "kernel/graphics/vgaconsts.h"
#include "kernel/std/mem.h"

#define VGA_WIDTH	80
#define VGA_HEIGHT  25
#define VGA_MEMORY  0xB8000

size_t vga_row;
size_t vga_column;
uint8_t vga_current_color;
uint16_t *vga_buffer = (uint16_t *) VGA_MEMORY;

void vga_init(void) 
{
	vga_row = 0;
	vga_column = 0;
	vga_current_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			vga_buffer[index] = vga_entry(' ', vga_current_color);
		}
	}
}

void vga_setcolor(uint8_t color) 
{
	vga_current_color = color;
}

void vga_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	vga_buffer[index] = vga_entry(c, color);
}

void vga_clear(void) {
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			vga_putentryat(' ', vga_current_color, x, y);
		}
	}

	vga_row = 0;
	vga_column = 0;
}

void vga_scroll() {
    for (int y = 1; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            vga_buffer[(y - 1) * VGA_WIDTH + x] = vga_buffer[y * VGA_WIDTH + x];
        }
    }
    
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        vga_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', vga_current_color);
    }
    
    vga_row = VGA_HEIGHT - 1;
    vga_column = 0;
}

void vga_write_char(char c) {
	switch(c) {
		case '\n':
			vga_column = 0;
			vga_row++;
			break;
		case '\r':
			vga_column = 0;
			break;
		case '\b':
			if(vga_column > 0) vga_column--;
			else if(vga_row > 0) {
				vga_row--;
				vga_column = VGA_WIDTH - 1;
			}

			vga_putentryat(0, vga_current_color, vga_column, vga_row);
			break;
		default:
			vga_putentryat(c, vga_current_color, vga_column, vga_row);
			if (++vga_column == VGA_WIDTH) {
				vga_column = 0;
				vga_row++;
			}
			break;
	}

	if (vga_row == VGA_HEIGHT) vga_scroll();
}

void vga_printf(const char *format, ...) {
	char *buf;
	int i;
	va_list args;
	va_start(args, format);

	while(*format) {
		if(*format == '\t') for (int i = 0; i < 4; i++) vga_write_char(' ');
		else if(*format == '%') {
			format++;
			switch(*format) {
				case 'c':
					i = va_arg(args, int);
					vga_write_char(i);
					break;
				case 'd':
					i = va_arg(args, int);
					if(i<0) {
						i = -i;
						vga_write_char('-');
					}
					
					buf = convert(i, 10);
					for (char *p = buf; *p; ++p) vga_write_char(*p);
					break;
				case 'o':
					i = va_arg(args, unsigned int);
					buf = convert(i, 8);
					for (char *p = buf; *p; ++p) vga_write_char(*p);
					break;
				case 'x':
					i = va_arg(args, unsigned int);
					buf = convert(i, 16);
					for (char *p = buf; *p; ++p) vga_write_char(*p);
					break;
				case 's':
					buf = va_arg(args, char *);
					for (char *p = buf; *p; ++p) vga_write_char(*p);
					break;
			}
		} else vga_write_char(*format);

		format++;
	}

	va_end(args);
}

void vga_putchar(char c) {
	vga_printf("%c", c);
}

void vga_write(const char *data, size_t size) {
	char buffer[size + 1];
	memcpy(buffer, data, size);
	buffer[size] = '\0';

	vga_printf("%s", buffer);
}

void vga_writestring(const char *data) {
	vga_printf("%s", data);
}

#endif // VGA_H
