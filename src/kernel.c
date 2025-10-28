#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "kernel/boot/limine.h"

__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(4);

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
	.id = LIMINE_FRAMEBUFFER_REQUEST,
	.revision = 0
};

__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;

static void hcf() { for(;;) __asm__("hlt"); }

void kmain() {
	if (LIMINE_BASE_REVISION_SUPPORTED == false || framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1)
		hcf();

	struct limine_framebuffer *fb = framebuffer_request.response->framebuffers[0];
	uint32_t *ptr = fb->address;

	for (uint64_t y = 0; y < fb->height; y++)
		for (uint64_t x = 0; x < fb->width; x++)
			ptr[y * (fb->pitch / 4) + x] = 0xFF69B4;

	hcf();
}
