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

void init_switch_screen(layer2_screen_t *screen);

void switch_top_screen_section(layer2_screen_t *screen);

void switch_middle_screen_section(layer2_screen_t *screen);

void switch_bottom_screen_section(layer2_screen_t *screen);

void end_switch_screen(layer2_screen_t *screen);

void switch_ram_bank(uint8_t bank);

#endif
