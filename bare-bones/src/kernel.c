#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "vgaio.h"
#include "vector.h"
#include "utils.h"
#include "pepe.h"
#include "ps2.h"
#include "multiboot2.h"
#include "fbout.h"


#if !defined(__i386__)
#error "This needs to be compiled with a ix86-elf compiler"
#endif

#define FOREACHTAG(tag, addr) for(struct multiboot_tag *tag = (struct multiboot_tag *)((uint8_t *)(addr) + 8); tag->type != MULTIBOOT_TAG_TYPE_END; tag = (struct multiboot_tag *)((uint8_t *)(tag) + ((tag->size + 7) & ~7)))

void kernel_main(multiboot_info_t* mbd, uint32_t magic) {
void kernel_main(uint32_t multiboot_addr, uint32_t magic) {
    vga_init();

    if(magic != MULTIBOOT_BOOTLOADER_MAGIC) {
    unsigned size = *(unsigned *) multiboot_addr;

    if(magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        vga_printf("Panic! MAGIC=0x%x", magic);
        while(true);
    }

    if(multiboot_addr & 7) {
        vga_printf("Unaligned mbi: 0x%x\n", multiboot_addr);
        while(true);
    }
        // GRAPHICS MODE
        uint8_t res = ps2_keyboard_init();
        if(res != 0) {
            fb_printf(&fb, "Something went wrong while initializing the PS/2 keyboard\n");
            fb_printf(&fb, "Error code: %d\n", res+'0');
            while(true);
        }

        res = ps2_keyboard_set_scan_code_set(2);
        if(res != 0) {
            fb_printf(&fb, "Something went wrong while setting scan code set to 2\n");
            fb_printf(&fb, "Error code: %d\n", res+'0');
            while(true);
        }

        fb_printf(&fb, "PS/2 keyboard initialized. Test your typing (F1=clear ESC=exit):\n");
        
        while (true) {
            uint8_t sc = _read_scan_code();
            char read = ps2_scan_code_to_char(sc, false);\
            
            if (sc == 0x01) break; // ESC
            if(sc == 0x3B) fb_clear(&fb); // F1
            if(read == 0) continue;
            
            fb_printf(&fb, "%c", read);
        }

        uint32_t color[14] = {
            C_RED, C_LIME, C_BLUE, C_YELLOW,
            C_CYAN, C_MAGENTA, C_SILVER, C_GRAY,
            C_MAROON, C_OLIVE, C_GREEN, C_PURPLE,
            C_TEAL, C_NAVY
        };
        
        int i = 0, j = 0;
        while(true) {
            fb_clear(&fb);
            fb_puts(&fb, frames[i], color[j]);
            //fb_printf(&fb, "%s", frames[i]);
            i = (i + 1) % FRAMES_COUNT;
            j = (i + 1) % 14;
            sleep(8000);
        }
    } else {
        // VGA TEXT MODE
        //uint32_t width = mbd->framebuffer_width;
        //uint32_t height = mbd->framebuffer_height;
        //uint8_t bpp = mbd->framebuffer_bpp;
        //vga_printf("Resolution: %dx%d\n", width, height);
        //vga_printf("BPP: %d\n", bpp);
        vga_printf("No framebuffer available, using VGA text mode\n");

        vga_clear();
        vga_printf("Keyboard testing\n");

        uint8_t res = ps2_keyboard_init();
        if(res != 0) {
            vga_printf("Something went wrong while initializing the PS/2 keyboard\n");
            vga_printf("Error code: %d\n", res+'0');
            while(true);
        }

        res = ps2_keyboard_set_scan_code_set(2);
        if(res != 0) {
            vga_printf("Something went wrong while setting scan code set to 2\n");
            vga_printf("Error code: %d\n", res+'0');
            while(true);
        }

        vga_printf("PS/2 keyboard initialized. Test your typing (F1=clear ESC=exit):\n");

        while (true) {
            uint8_t sc = _read_scan_code();
            char read = ps2_scan_code_to_char(sc, false);
            
            if (sc == 0x01) break; // ESC
            if(sc == 0x3B) vga_clear(); // F1
            if(read == 0) continue;
            
            vga_printf("%c", read);
        }

        int i = 0;
        while(true) {
            vga_clear();
            vga_setcolor(i+1);
            vga_printf("%s", frames[i]);
            i = (i + 1) % FRAMES_COUNT;
            sleep(8000);
        }
    }
}
