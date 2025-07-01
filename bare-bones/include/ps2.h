#include <stdint.h>
#include "ioports.h"

#define PS2_DATA_PORT 0x60
#define PS2_STATUS_REGISTER 0x64
#define PS2_COMMAND_REGISTER 0x64

/*
	PS/2 Status Register queries
*/

static inline uint8_t ps2_output_buffer_status() {
	return inb(PS2_DATA_PORT) & 1;
}

static inline uint8_t ps2_input_buffer_status() {
	return inb(PS2_DATA_PORT) & 2;
}

static inline uint8_t ps2_system_flag() {
	return inb(PS2_DATA_PORT) & 4;
}

static inline uint8_t ps2_command_or_data() {
	return inb(PS2_DATA_PORT) & 8;
}

static inline uint8_t ps2_chipset_specific() {
	return inb(PS2_DATA_PORT) & 16;
}

static inline uint8_t ps2_chipset_specific_2() {
	return inb(PS2_DATA_PORT) & 32;
}

static inline uint8_t ps2_timeout_error() {
	return inb(PS2_DATA_PORT) & 64;
}

static inline uint8_t ps2_parity_error() {
	return inb(PS2_DATA_PORT) & 128;
}

/*
	PS/2 Controller Commands
*/

// TODO