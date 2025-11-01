#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <kernel/cpu/dump.h>
#include <std/misc.h>
#include <kernel/graphics/framebuffer.h>
#include <kernel/boot/limine.h>

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

	dump_cr(fb);
	fb_printf(fb, "\n", NULL);
	dump_fb(fb);

	hcf();
}
