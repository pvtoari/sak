#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>

#include "graphics/colors.h"
#include "std/string.h"
#include "std/misc.h"
#include "math/math.h"
#include "kernel/boot/limine.h"
#include "kernel/psf/cp850_8x16.h"
#include "kernel/std/mem.h"

static size_t _char_row = 0;
static size_t _char_column = 0;

typedef struct limine_framebuffer framebuffer;

void fb_put_pixel(framebuffer *fb, uint32_t x, uint32_t y, uint32_t color);

uint32_t fb_get_pixel(framebuffer *fb, uint32_t x, uint32_t y);

static inline uint8_t *get_glyph(unsigned char c) {
    return &psf[PSF_HEADER_SIZE + c * GLYPH_BYTES];
}

void fb_fill(framebuffer *fb, uint32_t width, uint32_t height, uint32_t color);

void fb_draw_line(framebuffer *fb, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t color);

void fb_draw_rectangle(framebuffer *fb, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t color);

void _fb_raw_putchar(framebuffer *fb, uint32_t x, uint32_t y, unsigned char c, uint32_t color);

void fb_scroll(framebuffer *fb);

void fb_putchar(framebuffer *fb, unsigned char c, uint32_t color);

void fb_puts(framebuffer *fb, const char *str, uint32_t color);

void fb_printf(framebuffer *fb, const char *format, ...);

void fb_clear(framebuffer *fb);

#endif // FRAMEBUFFER_H
