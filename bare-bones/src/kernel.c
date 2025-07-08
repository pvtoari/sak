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
#include "acpi.h"


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
