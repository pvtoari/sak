#include <io/ps2.h>

uint8_t ps2_keyboard_init() {
    int retries = 3;

reset:
    ps2_send_first_port_command(PS2_COMMAND_RESET_AND_SELFTEST);
    while (!ps2_output_buffer_status());
    
    uint8_t res = ps2_get_data_byte();
    if (res == PS2_RESPONSE_RESEND && --retries > 0) goto reset;
    if (res != PS2_RESPONSE_ACK) return 1;
    
    while (!ps2_output_buffer_status());
    if (ps2_get_data_byte() != PS2_RESPONSE_SELF_TEST_PASSED) return 2;

	while (ps2_input_buffer_status());
	ps2_send_first_port_command(PS2_COMMAND_SET_DEFAULT_PARAMETERS);
	while (!ps2_output_buffer_status());
	if (ps2_get_data_byte() != PS2_RESPONSE_ACK) return 3;

    ps2_send_first_port_command(PS2_COMMAND_ENABLE_SCANNING);
    while (!ps2_output_buffer_status());
    if (ps2_get_data_byte() != PS2_RESPONSE_ACK) return 4;

    return 0;
}

uint8_t ps2_read_scan_code(bool *released, bool *extended) {
	// stupid shit that doesnt work
    static bool got_f0 = false;
    static bool got_e0 = false;

    while (true) {
        uint8_t sc = ps2_read_data();

        if (sc == 0xF0) {
            got_f0 = true;
            continue;
        }

        if (sc == 0xE0) {
            got_e0 = true;
            continue;
        }

        *released = got_f0;
        *extended = got_e0;

        got_f0 = false;
        got_e0 = false;

        return sc;
    }
}

uint8_t ps2_keyboard_set_scan_code_set(uint8_t set) {
	// doesnt work
	while (ps2_input_buffer_status());
	ps2_send_first_port_command(PS2_COMMAND_SET_CURRENT_SCAN_CODE_SET);
	while (!ps2_output_buffer_status());
	if (ps2_get_data_byte() != PS2_RESPONSE_ACK) return 1;

	while (ps2_input_buffer_status());
	ps2_send_first_port_command(set);
	while (!ps2_output_buffer_status());
	if (ps2_get_data_byte() != PS2_RESPONSE_ACK) return 2;

	return 0;
}

uint8_t ps2_mouse_init() {
    ps2_send_controller_command(PS2_COMMAND_ENABLE_SECOND_PORT);
    ps2_output_buffer_flush();

    int retries = 3;
reset:
    while (ps2_input_buffer_status());
    ps2_send_second_port_command(PS2_COMMAND_RESET_AND_SELFTEST);

    while (!ps2_output_buffer_status());
    uint8_t res = ps2_get_data_byte();
    if (res == PS2_RESPONSE_RESEND && --retries > 0) goto reset;
    if (res != PS2_RESPONSE_ACK) return 1;

    while (!ps2_output_buffer_status());
    res = ps2_get_data_byte();
    if (res == PS2_RESPONSE_SELF_TEST_FAILED) return 2;
    if (res == PS2_RESPONSE_SELF_TEST_FAILED_2) return 3;

    while (ps2_input_buffer_status());
    ps2_send_second_port_command(PS2_COMMAND_SET_DEFAULT_PARAMETERS);

    while (!ps2_output_buffer_status());
    res = ps2_get_data_byte();
    if (res == PS2_RESPONSE_SELF_TEST_FAILED) return 4;
    if (res == PS2_RESPONSE_SELF_TEST_FAILED_2) return 5;

    return 0;
}

uint8_t ps2_mouse_set_sample_rate(uint8_t rate) {
    while (ps2_input_buffer_status());
    ps2_send_second_port_command(PS2_MOUSE_COMMAND_SET_SAMPLE_RATE);

    while (!ps2_output_buffer_status());
    uint8_t res = ps2_get_data_byte();
    if (res != PS2_RESPONSE_ACK) return 1;

    while (ps2_input_buffer_status());
    ps2_send_second_port_command(rate);

    while (!ps2_output_buffer_status());
    res = ps2_get_data_byte();
    if (res != PS2_RESPONSE_ACK) return 2;

    return 0;
}

uint8_t ps2_mouse_enable_scroll_wheel() {
    ps2_output_buffer_flush();

    uint8_t res = ps2_mouse_set_sample_rate(
        PS2_MOUSE_SUBCOMMAND_SET_SAMPLE_RATE_TWO_HUNDRED);
    if (res != 0) return 1;

    res = ps2_mouse_set_sample_rate(
        PS2_MOUSE_SUBCOMMAND_SET_SAMPLE_RATE_ONE_HUNDRED);
    if (res != 0) return 2;

    res = ps2_mouse_set_sample_rate(
        PS2_MOUSE_SUBCOMMAND_SET_SAMPLE_RATE_EIGHTY);
    if (res != 0) return 3;

    ps2_output_buffer_flush();

    while (ps2_input_buffer_status());
    ps2_send_second_port_command(PS2_MOUSE_COMMAND_GET_MOUSE_ID);

    while (!ps2_output_buffer_status());
    res = ps2_get_data_byte();
    if (res != PS2_RESPONSE_ACK) return 4;

    while (!ps2_output_buffer_status());
    res = ps2_get_data_byte();

    if (res == PS2_MOUSE_ID_THREE_BUTTONS) return 5;
    if (res == PS2_MOUSE_ID_FIVE_BUTTONS) return 6;

    return 0;
}

uint8_t ps2_mouse_enable_packet_streaming() {
    ps2_output_buffer_flush();

    while (ps2_input_buffer_status());
    ps2_send_second_port_command(PS2_MOUSE_COMMAND_ENABLE_PACKET_STREAMING);

    while (!ps2_output_buffer_status());
    uint8_t res = ps2_get_data_byte();
    if (res != PS2_RESPONSE_ACK) return 1;

    return 0;
}

uint8_t ps2_mouse_disable_packet_streaming() {
    ps2_output_buffer_flush();

    while (ps2_input_buffer_status());
    ps2_send_second_port_command(PS2_MOUSE_COMMAND_DISABLE_PACKET_STREAMING);

    while (!ps2_output_buffer_status());
    uint8_t res = ps2_get_data_byte();
    if (res != PS2_RESPONSE_ACK) return 1;

    return 0;
}