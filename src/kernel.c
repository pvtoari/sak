#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "graphics/vga.h"
#include "graphics/framebuffer.h"
#include "math/geometry.h"
#include "std/utils.h"
#include "sys/ps2.h"
#include "sys/multiboot2.h"
#include "sys/acpi.h"
#include "math/vector.h"
#include "raw/pepe.h"
#include "raw/chudjackr.h"

#define STBI_NO_STDIO
#define STBI_NO_HDR
#define STBI_ASSERT
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_ZLIB
#define STBI_NO_THREAD_LOCALS
#define STBI_NO_FAILURE_STRINGS
#define STBI_NO_LINEAR
#define STBI_NO_HDR
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_PIC
#define STBI_NO_PNM

#include "stb_image.h"

char *stbi_zlib_decode_malloc_guesssize_headerflag(const char *buffer, int len, int initial_size, int *outlen, int parse_header) {
    return 0;
}

#if !defined(__i386__)
#error "This needs to be compiled with a ix86-elf compiler"
#endif

#define FOREACHTAG(tag, addr) for(struct multiboot_tag *tag = (struct multiboot_tag *)((uint8_t *)(addr) + 8); tag->type != MULTIBOOT_TAG_TYPE_END; tag = (struct multiboot_tag *)((uint8_t *)(tag) + ((tag->size + 7) & ~7)))

FADT_t *findFACP(void *RootSDT) {
    RSDT_t *rsdt = (RSDT_t *) RootSDT;
    int entries = (rsdt->h.Length - sizeof(rsdt->h)) / 4;

    for (int i = 0; i < entries; i++)
    {
        ACPISDTHeader_t *h = (ACPISDTHeader_t *) rsdt->PointerToOtherSDT[i];
        if (!strcmp(h->Signature, "FACP"))
            return (void *) h;
    }

    // No FACP found
    return NULL;
}

void cube(framebuffer *fb) {
    const uint32_t width = fb->width;
    const uint32_t height = fb->height;
    const uint32_t scale = 300;
    const uint32_t color = C_WHITE;
    const uint32_t bg = C_BLACK;
    const double inc = 0.01f;
    const double threshold = 1000;
    
    vec3 cube[8] = {
        {-1, -1, -1}, {1, -1, -1},
        {1,  1, -1}, {-1,  1, -1},
        {-1, -1,  1}, {1, -1,  1},
        {1,  1,  1}, {-1,  1,  1}
    };
    
    edge edges[12] = {
        {0,1},{1,2},{2,3},{3,0},
        {4,5},{5,6},{6,7},{7,4},
        {0,4},{1,5},{2,6},{3,7}
    };

    vec2 prev[8] = {0};

    vec3 transformed[8];
    vec2 projected[8];
    double angle = 0.0;
    double z_offset = (width + height)/(2 * scale);
    while(true) {
        for(int i = 0; i < 8; i++) {
            transformed[i] = cube[i];
            
            rotate_y(&transformed[i], angle);
            rotate_x(&transformed[i], angle);
            rotate_z(&transformed[i], angle);
            projected[i] = project(&transformed[i], width, height, scale, z_offset);
        }

        for(int i = 0; i < 12; i++) {
            vec2 a = prev[edges[i].a];
            vec2 b = prev[edges[i].b];

            fb_draw_line(fb, a.x, a.y, b.x, b.y, bg);
        }    

        for(int i = 0; i < 12; i++) {
            vec2 a = projected[edges[i].a];
            vec2 b = projected[edges[i].b];

            fb_draw_line(fb, a.x, a.y, b.x, b.y, color);
        }

        for(int i = 0; i < 8; i++) prev[i] = projected[i];

        angle += inc;
        sleep(threshold);
    }
}

void plane(framebuffer *fb) {
    const uint32_t width = fb->width;
    const uint32_t height = fb->height;
    const uint32_t scale = 200;
    const uint32_t color = C_WHITE;
    const uint32_t bg = C_BLACK;
    const double inc = 0.01f;
    const double threshold = 1000;
    
    static vec3 plane[4] = {
        {-1, -1, 0},
        { 1, -1, 0},
        { 1,  1, 0},
        {-1,  1, 0}
    };
    
    static edge edges[4] = {
        {0,1}, {1,2}, {2,3}, {3,0}
    };

    static vec2 prev[4] = {0};

    vec3 transformed[4];
    vec2 projected[4];
    double angle = 0.0;
    double z_offset = (width + height)/(2 * scale);
    while(true) {
        for(int i = 0; i < 4; i++) {
            transformed[i] = plane[i];
            
            rotate_y(&transformed[i], angle);
            rotate_x(&transformed[i], angle);
            rotate_z(&transformed[i], angle);
            projected[i] = project(&transformed[i], width, height, scale, z_offset);
        }

        for(int i = 0; i < 4; i++) {
            vec2 a = prev[edges[i].a];
            vec2 b = prev[edges[i].b];

            fb_draw_line(fb, a.x, a.y, b.x, b.y, bg);
        }    

        for(int i = 0; i < 4; i++) {
            vec2 a = projected[edges[i].a];
            vec2 b = projected[edges[i].b];

            fb_draw_line(fb, a.x, a.y, b.x, b.y, color);
        }

        for(int i = 0; i < 4; i++) prev[i] = projected[i];

        angle += inc;
        sleep(threshold);
    }
}

void kernel_main(uint32_t multiboot_addr, uint32_t magic) {
    vga_init();

    unsigned size = *(unsigned *) multiboot_addr;

    if(magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        vga_printf("Panic! MAGIC=0x%x", magic);
        while(true);
    }

    if(multiboot_addr & 7) {
        vga_printf("Unaligned mbi: 0x%x\n", multiboot_addr);
        while(true);
    }
    
    framebuffer fb;
    XSDP_t *rsdp = NULL;
    FADT_t* fadt = NULL;
    
    bool fb_found = false;
    FOREACHTAG(tag, multiboot_addr) {
        switch(tag->type) {
            case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
                struct multiboot_tag_framebuffer_common *fbtag = (struct multiboot_tag_framebuffer_common *) tag;
                if (fbtag->framebuffer_type != MULTIBOOT_FRAMEBUFFER_TYPE_RGB) {
                    vga_printf("Unsupported framebuffer type: %d\n", fbtag->framebuffer_type);
                    while (1);
                }

                fb.width = fbtag->framebuffer_width;
                fb.height = fbtag->framebuffer_height;
                fb.bpp = fbtag->framebuffer_bpp;
                fb.pitch = fbtag->framebuffer_pitch;
                fb.addr = (uint32_t *)(uintptr_t)fbtag->framebuffer_addr;
            
                fb_printf(&fb, "Framebuffer found!\n");
                fb_printf(&fb, "Framebuffer width: %d\n", fb.width);
                fb_printf(&fb, "Framebuffer height: %d\n", fb.height);
                fb_printf(&fb, "Framebuffer bpp: %d\n", fb.bpp);
                fb_printf(&fb, "Framebuffer pitch: %d\n", fb.pitch);
                fb_printf(&fb, "Framebuffer address: 0x%x\n", fb.addr);
                
                fb_found = true;
                break;
            case MULTIBOOT_TAG_TYPE_ACPI_NEW:
                struct multiboot_tag_new_acpi *new_acpi = (struct multiboot_tag_new_acpi *) tag;
                rsdp = (XSDP_t *) new_acpi->rsdp;
                break;
            case MULTIBOOT_TAG_TYPE_ACPI_OLD:
                struct multiboot_tag_old_acpi *old_acpi = (struct multiboot_tag_old_acpi *) tag;
                rsdp = (XSDP_t *) old_acpi->rsdp;
                break;
        }
    }

    if(!rsdp) fb_printf(&fb, "ACPI RSDP not found\n");
    else {
        void *root_sdt = (void *)(uintptr_t) (rsdp->xsdt_address ? rsdp->xsdt_address : rsdp->rsdt_address);
        fadt = (FADT_t *) findFACP(root_sdt);
        fb_printf(&fb, "ACPI RSDP revision: %s\n", rsdp->revision == 0 ? "1.0" : ">1.0");
        if(!fadt) fb_printf(&fb, "FADT not found");
        else {
            fb_printf(&fb, "FADT Revision: %d\n", fadt->h.Revision);
            fb_printf(&fb, "FirmwareCtrl: 0x%x\n", fadt->FirmwareCtrl);
            fb_printf(&fb, "DSDT Address: 0x%x\n", fadt->Dsdt);
            fb_printf(&fb, "Preferred PM Profile: %d\n", fadt->PreferredPowerManagementProfile);
            fb_printf(&fb, "SCI Interrupt: %d\n", fadt->SCI_Interrupt);
            fb_printf(&fb, "SMI Command Port: 0x%x\n", fadt->SMI_CommandPort);
            fb_printf(&fb, "ACPI Enable: 0x%x\n", fadt->AcpiEnable);
            fb_printf(&fb, "ACPI Disable: 0x%x\n", fadt->AcpiDisable);
            fb_printf(&fb, "Reset Register Address: 0x%x\n", fadt->ResetReg.Address);
            fb_printf(&fb, "Reset Value: 0x%x\n", fadt->ResetValue);
        }
    }
    
    if(fb_found) {
        // GRAPHICS MODE
        uint8_t res = ps2_keyboard_init();
        if(res != 0) {
            fb_printf(&fb, "Something went wrong while initializing the PS/2 keyboard\n");
            fb_printf(&fb, "Error code: %d\n", res);
            while(true);
        }

        res = ps2_keyboard_set_scan_code_set(2);
        if(res != 0) {
            fb_printf(&fb, "Something went wrong while setting scan code set to 2\n");
            fb_printf(&fb, "Error code: %d\n", res);
            while(true);
        }
        
        res = ps2_mouse_init();
        if(res != 0) {
            fb_printf(&fb, "Something went wrong while initializing the PS/2 mouse\n");
            fb_printf(&fb, "Error code: %d\n", res);
            while(true);
        }

        res = ps2_mouse_set_sample_rate(PS2_MOUSE_SUBCOMMAND_SET_SAMPLE_RATE_FORTY);
        if(res != 0) {
            fb_printf(&fb, "Something went wrong while setting sample rate to 40\n");
            fb_printf(&fb, "Error code: %d\n", res);
            while(true);
        }

        // res = ps2_mouse_enable_scroll_wheel();
        // if(res != 0) {
        //     fb_printf(&fb, "Something went wrong while enabling scroll wheel\n");
        //     fb_printf(&fb, "Error code: %d\n", res);
        //     while(true);
        // }

        res = ps2_mouse_enable_packet_streaming();
        if(res != 0) {
            fb_printf(&fb, "Something went wrong while enabling PS/2 mouse packet streaming\n");
            fb_printf(&fb, "Error code: %d\n", res);
            while(true);
        }
        
        res = ps2_mouse_disable_packet_streaming();
        if(res != 0) {
            fb_printf(&fb, "Something went wrong while disabling PS/2 mouse packet streaming\n");
            fb_printf(&fb, "Error code: %d\n", res);
            while(true);
        }
        
        fb_printf(&fb, "PS/2 keyboard initialized. Test your typing (F1=clear ESC=exit):\n");
        
        while (true) {
            uint8_t sc = ps2_read_data();

            if (ps2_data_from_mouse()) {
                uint8_t packet[4];
                packet[0] = sc;
        
                for (int i = 1; i < 4; i++) {
                    while (!ps2_output_buffer_status());
                    packet[i] = ps2_read_data();
                }
                
                uint8_t left_button_down = ps2_mouse_packet_first_byte_left_button_down(packet[0]);
                uint8_t right_button_down = ps2_mouse_packet_first_byte_right_button_down(packet[0]);
                uint8_t middle_button_down = ps2_mouse_packet_first_byte_middle_button_down(packet[0]);
                uint8_t x_sign = ps2_mouse_packet_first_byte_x_sign(packet[0]);
                uint8_t y_sign = ps2_mouse_packet_first_byte_y_sign(packet[0]);
                uint8_t x_overflow = ps2_mouse_packet_first_byte_x_overflow(packet[0]);
                uint8_t y_overflow = ps2_mouse_packet_first_byte_y_overflow(packet[0]);
        
                fb_printf(&fb, "Mouse Packet:\n");
                fb_printf(&fb, "Left btn: %d, Right btn: %d, Middle btn: %d\n", left_button_down, right_button_down, middle_button_down);
                fb_printf(&fb, "x sign: %d, y sign: %d\n", x_sign, y_sign);
                fb_printf(&fb, "x overflow: %d, y overflow: %d\n", x_overflow, y_overflow);
                fb_printf(&fb, "x movement: %d, y movement: %d\n", (int8_t)packet[1], (int8_t)packet[2]);
                fb_printf(&fb, "Scroll: %d\n", (int8_t)packet[3]);
            } else {
                if (sc == 0x01) break; // ESC
                if (sc == 0x3B) fb_clear(&fb); // F1
                if (sc == 0x3D) ps2_mouse_enable_packet_streaming(); // F3
                if (sc == 0x3E) ps2_mouse_disable_packet_streaming(); // F4
                
                char read = ps2_scan_code_to_char(sc, false);
                if (read != 0) fb_printf(&fb, "%c", read);
            }
        }

        //plane(&fb);
        cube(&fb);

        int width, height, channels;
        unsigned char *image = stbi_load_from_memory(
            chudjackr_jpg,
            chudjackr_jpg_len,
            &width, &height, &channels,
            4
        );

        if(!image) {
            fb_printf(&fb, "chudjack could not be loaded :(");
            while(true);
        }
        
        fb_printf(&fb, "Loaded image: %dx%d (%d channels)\n", width, height, channels);
        fb_printf(&fb, "Chudjack pointer: 0x%x  | sizeof pointer: %d \n", image, sizeof(image));

        uint32_t x_offset = ((fb.width > width) ? (fb.width - width) / 2 : 0);
        uint32_t y_offset = ((fb.height > height) ? (fb.height - height) / 2 : 0) + 340;
        fb_printf(&fb, "Printing at x:%d y:%d\n", x_offset, y_offset);

        while(true) {
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    unsigned char* pixel = &image[(y * width + x) * 4];
                    uint8_t r = pixel[0];
                    uint8_t g = pixel[1];
                    uint8_t b = pixel[2];
                    // uint8_t a = pixel[3]; // JPG no tiene, puedes ignorar
        
                    uint32_t color = (r << 16) | (g << 8) | b;
                    for(int i = 0; i < 12; i++) {
                        fb_put_pixel(&fb, x + x_offset+16*i, y + y_offset-64*i, color);
                    }
                }
            }

            x_offset = (x_offset + 1) % fb.width;
            sleep(300);
        }

        fb_printf(&fb, "Done!");

        while(true);         

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
    }
}
