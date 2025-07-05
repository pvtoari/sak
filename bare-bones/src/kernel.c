#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "vga.h"
#include "vector.h"
#include "utils.h"
#include "pepe.h"

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif



    }




        }


void vector_test() {
	uint32_t sleep_amount = 100000;
    vector v;
    vector_init(&v);
    
    terminal_writestring("\nStarting vector tests...\n\n");
    sleep(sleep_amount);
    
    terminal_writestring("Adding 10 elements...\n");
    for(uint32_t i = 0; i < 10; i++) {
        v.add(&v, (void*)(i + 1));
        show_vector_status(&v, "test");
        sleep(sleep_amount);
    }

    }
}
