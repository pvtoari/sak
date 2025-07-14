#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
//#include "psf/anniki8x8.h"
#include "psf/anniki16x16.h"
#include <stdbool.h>
#include "std/mem.h"
#include "std/string.h"
#include "std/utils.h"
#include "math/math.h"
#include "graphics/colors.h"

typedef struct framebuffer {
    uint32_t width;
    uint32_t height;
    uint32_t bpp;
    uint32_t pitch;
    uint32_t *addr;
} framebuffer;

static size_t _char_row = 0;
static size_t _char_column = 0;

static inline void fb_put_pixel(framebuffer* fb, uint32_t x, uint32_t y, uint32_t color) {
    fb->addr[y * (fb->pitch / sizeof(uint32_t)) + x] = color;
}

static inline uint32_t fb_get_pixel(framebuffer* fb, uint32_t x, uint32_t y) {
    return fb->addr[y * (fb->pitch / sizeof(uint32_t)) + x];
}

static inline uint8_t *get_glyph(unsigned char c) {
    return &psf[PSF_HEADER_SIZE + c * GLYPH_BYTES];
}

void fb_fill(framebuffer *fb, uint32_t width, uint32_t height, uint32_t color) {
    for (uint32_t y = 0; y < height; y++) {
        for (uint32_t x = 0; x < width; x++) {
            fb_put_pixel(fb, x, y, color);
        }
    }
}

void _fb_raw_putchar(framebuffer *fb, uint32_t x, uint32_t y, unsigned char c, uint32_t color) {
    uint8_t *glyph = get_glyph(c);

#if GLYPH_WIDTH == 8 && GLYPH_HEIGHT == 8
    for (int row = 0; row < GLYPH_HEIGHT; row++) {
        uint8_t bits = glyph[row];
        for (int col = 0; col < 8; col++) {
            if (bits & (1 << (7 - col))) {
                fb_put_pixel(fb, x + col, y + row, color);
            }
        }
    }

#elif GLYPH_WIDTH == 16 && GLYPH_HEIGHT == 16
    for (int row = 0; row < GLYPH_HEIGHT; row++) {
        uint16_t bits = ((uint16_t) glyph[row * 2] << 8) | glyph[row * 2 + 1];

        for (int col = 0; col < 16; col++) {
            if (bits & (1 << (15 - col))) {
                fb_put_pixel(fb, x + col, y + row, color);
            }
        }
    }

#else
    #error "Unsupported dimensions for such PSF font"
#endif
}

void fb_scroll(framebuffer *fb) {
    const uint32_t max_rows = fb->height / GLYPH_HEIGHT;
    const uint32_t last_line = GLYPH_HEIGHT * (max_rows - 1);
    
    for (uint32_t y = 0; y < last_line; y++) {
        for (uint32_t x = 0; x < fb->width; x++) {
            uint32_t color = fb_get_pixel(fb, x, (y + GLYPH_HEIGHT));
            fb_put_pixel(fb, x, y, color);
        }
    }
    
    for (uint32_t y = 0; y < GLYPH_HEIGHT; y++) {
        for (uint32_t x = 0; x < fb->width; x++) {
            fb_put_pixel(fb, x, last_line + y, C_BLACK);
        }
    }

    _char_row = max_rows - 1;
    _char_column = 0;
}


void fb_putchar(framebuffer *fb, unsigned char c, uint32_t color) {
    const uint32_t max_columns = fb->width / GLYPH_WIDTH, max_rows = fb->height / GLYPH_HEIGHT;
    uint32_t x, y;
    
    switch(c) {
        case '\n':
            _char_column = 0;
            _char_row++;
            break;
        case '\r':
            _char_column = 0;
            break;
        case '\b':
            if(_char_column > 0) _char_column--;
            else if(_char_row > 0) {
                _char_row = max(0, --_char_row);
                _char_column = max_columns - 1;
            }

            x = _char_column * GLYPH_WIDTH, y = _char_row * GLYPH_HEIGHT;
            for (uint32_t row = 0; row < GLYPH_HEIGHT; row++) {
                for (uint32_t col = 0; col < GLYPH_WIDTH; col++) {
                    fb_put_pixel(fb, x + col, y + row, C_BLACK);
                }
            }
            
            break;
        case '\t':
            // TODO
            break;
        default:
            x = _char_column * GLYPH_WIDTH, y = _char_row * GLYPH_HEIGHT;
            
            _fb_raw_putchar(fb, x, y, c, color);
            if(++_char_column == max_columns) {
                _char_column = 0;
                _char_row++;
            }

            break;
    }

    if(_char_row == max_rows) fb_scroll(fb);
}

void fb_puts(framebuffer *fb, const char *str, uint32_t color) {
    while (*str) {
        fb_putchar(fb, *str, color);
        str++;
    }
}

void fb_printf(framebuffer *fb, const char *format, ...) {
	char *buf;
	int i;
	va_list args;
	va_start(args, format);

	while(*format) {
		if(*format == '%') {
			format++;
			switch(*format) {
				case 'c':
					i = va_arg(args, int);
					fb_putchar(fb, i, C_WHITE);
					break;
				case 'd':
					i = va_arg(args, int);
					if(i<0) {
						i = -i;
						fb_putchar(fb, '-', C_WHITE);
					}
					
					buf = convert(i, 10);
					fb_puts(fb, buf, C_WHITE);
					break;
				case 'o':
					i = va_arg(args, unsigned int);
					buf = convert(i, 8);
					fb_puts(fb, buf, C_WHITE);
					break;
				case 'x':
					i = va_arg(args, unsigned int);
					buf = convert(i, 16);
					fb_puts(fb, buf, C_WHITE);
					break;
				case 's':
					buf = va_arg(args, char *);
					fb_puts(fb, buf, C_WHITE);
					break;
			}
		} else fb_putchar(fb, *format, C_WHITE);

		format++;
	}

	va_end(args);
}

void fb_clear(framebuffer *fb) {
    fb_fill(fb, fb->width, fb->height, C_BLACK);

    _char_column = 0;
    _char_row = 0;
}

#endif // FRAMEBUFFER_H
