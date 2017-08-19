/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Definitions of common functions and variables used by the implementation of
 * zxnext_layer2.h.
 ******************************************************************************/

#ifndef _LAYER2_COMMON_H
#define _LAYER2_COMMON_H

#include <stdint.h>
#include "zxnext_layer2.h"

extern uint8_t buf_256[];

void init_switch_screen(off_screen_buffer_t *off_screen_buffer);

void switch_top_screen_section(off_screen_buffer_t *off_screen_buffer);

void switch_middle_screen_section(off_screen_buffer_t *off_screen_buffer);

void switch_bottom_screen_section(off_screen_buffer_t *off_screen_buffer);

void end_switch_screen(off_screen_buffer_t *off_screen_buffer);

void switch_ram_bank(uint8_t bank);

void switch_rom_bank(uint8_t bank);

#endif
