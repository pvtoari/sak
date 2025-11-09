#ifndef DUMP_H
#define DUMP_H

#include "kernel/graphics/framebuffer.h"
#include "kernel/cpu/cr.h"

#ifdef FRAMEBUFFER_H
#define printf(fb, format, ...) fb_printf(fb, format, __VA_ARGS__)
#define printfield(fb, field) printf(fb, "%s: %d\n", #field, field);
#define printbit(fb, CR, FLAG) printf(fb, "%s -> %s: 0x%x\n", #CR, #FLAG, CR & FLAG);
#else
#error "No printing function provided"
#endif

static inline void dump_cr(framebuffer *fb) {
    uint64_t cr0 = read_cr0();
    uint64_t cr2 = read_cr2();
    uint64_t cr3 = read_cr3();
    uint64_t cr4 = read_cr4();

    printf(fb, "CR0: 0x%x\n", cr0);
    printbit(fb, cr0, CR0_PE);
    printbit(fb, cr0, CR0_MP);
    printbit(fb, cr0, CR0_EM);
    printbit(fb, cr0, CR0_TS);
    printbit(fb, cr0, CR0_ET);
    printbit(fb, cr0, CR0_NE);
    printbit(fb, cr0, CR0_WP);
    printbit(fb, cr0, CR0_AM);
    printbit(fb, cr0, CR0_NW);
    printbit(fb, cr0, CR0_CD);
    printbit(fb, cr0, CR0_PG);

    printf(fb, "\n", NULL);

    printf(fb, "CR2: 0x%x\n", cr2);

    printf(fb, "\n", NULL);

    printf(fb, "CR3: 0x%x\n", cr3);
    printbit(fb, cr3, CR3_PWT);
    printbit(fb, cr3, CR3_PCD);
    printbit(fb, cr3, CR3_PCID);
    printbit(fb, cr3, CR3_PML4_PBA);

    printf(fb, "\n", NULL);

    printf(fb, "CR4: 0x%x\n", cr4);
    printbit(fb, cr4, CR4_VME);
    printbit(fb, cr4, CR4_PVI);
    printbit(fb, cr4, CR4_TSD);
    printbit(fb, cr4, CR4_DE);
    printbit(fb, cr4, CR4_PSE);
    printbit(fb, cr4, CR4_PAE);
    printbit(fb, cr4, CR4_MCE);
    printbit(fb, cr4, CR4_PGE);
    printbit(fb, cr4, CR4_PCE);
    printbit(fb, cr4, CR4_OSFXSR);
    printbit(fb, cr4, CR4_OSXMMEXCPT);
    printbit(fb, cr4, CR4_UMIP);
    printbit(fb, cr4, CR4_LA57);
    printbit(fb, cr4, CR4_VMXE);
    printbit(fb, cr4, CR4_SMXE);
    printbit(fb, cr4, CR4_FSGSBASE);
    printbit(fb, cr4, CR4_PCIDE);
    printbit(fb, cr4, CR4_OSXSAVE);
    printbit(fb, cr4, CR4_SMEP);
    printbit(fb, cr4, CR4_SMAP);
    printbit(fb, cr4, CR4_PKE);
    printbit(fb, cr4, CR4_CET);
    printbit(fb, cr4, CR4_PKS);
}

static inline void dump_fb(framebuffer *fb) {
    printf(fb, "framebuffer info:\n", NULL);
    printf(fb, "fb->address: 0x%x\n", fb->address);
    printfield(fb, fb->width);
    printfield(fb, fb->height);
    printfield(fb, fb->pitch);
    printfield(fb, fb->bpp);
    printfield(fb, fb->memory_model);
    printfield(fb, fb->red_mask_size);
    printfield(fb, fb->red_mask_shift);
    printfield(fb, fb->green_mask_size);
    printfield(fb, fb->green_mask_shift);
    printfield(fb, fb->blue_mask_size);
    printfield(fb, fb->blue_mask_shift);
    printfield(fb, fb->edid_size);
    printfield(fb, fb->edid);
}

#endif // DUMP_H
