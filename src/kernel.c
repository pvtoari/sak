#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <io/ps2.h>
#include <kernel/boot/limine.h>
#include <kernel/cpu/dump.h>
#include <kernel/graphics/framebuffer.h>
#include <std/misc.h>

__attribute__((used, section(".limine_requests"))) static volatile LIMINE_BASE_REVISION(4);

__attribute__((used, section(".limine_requests"))) static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0};

__attribute__((used, section(".limine_requests"))) static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0};

__attribute__((used, section(".limine_requests"))) static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0};

__attribute__((used, section(".limine_requests_start"))) static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end"))) static volatile LIMINE_REQUESTS_END_MARKER;

static void hcf() {
    for (;;) __asm__("hlt");
}

static inline void await_enter(framebuffer *fb) {
    fb_printf(fb, "\n> Press enter...\n");
    while (true) {
        uint8_t sc = ps2_read_data();
        if (sc == 0x1C) break;
    }
}

void kmain() {
    uint8_t res = ps2_keyboard_init();
    if (LIMINE_BASE_REVISION_SUPPORTED == false || framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1)
        hcf();

    struct limine_framebuffer *fb = framebuffer_request.response->framebuffers[0];

    dump_cr(fb);
    await_enter(fb);

    fb_printf(fb, "\n");
    dump_fb(fb);
    await_enter(fb);
    fb_printf(fb, "\n");
    
    uint8_t entries = memmap_request.response->entry_count;
    fb_printf(fb, "number of limine memory mapped entries: %d\n", entries);
    for (uint64_t i = 0; i < entries; i++) {
        fb_printf(fb, "entry %d | type %d: 0x%x-0x%x\n", i,
            memmap_request.response->entries[i]->type,
            memmap_request.response->entries[i]->base,
            memmap_request.response->entries[i]->base + memmap_request.response->entries[i]->length);
    }

    hcf();
}
