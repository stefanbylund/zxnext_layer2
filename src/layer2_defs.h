/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Common definitions used by the implementation of zxnext_layer2.h.
 ******************************************************************************/

#ifndef _LAYER2_DEFS_H
#define _LAYER2_DEFS_H

#include <stddef.h>
#include <stdint.h>
#include "zxnext_registers.h"

/*
 * (R/W) Layer 2 screen configuration port (0x123B):
 * bits 6-7 = Section (0 = top, 1 = middle, 2 = bottom) of the specified layer 2
 * screen (main or shadow) to page in for writing.
 * bits 4-5 = Reserved, must be 0.
 * bit 3 = If 0, page in main layer 2 screen as specified by LAYER2_RAM_PAGE_REGISTER.
 * If 1, page in shadow layer 2 screen as specified by LAYER2_SHADOW_RAM_PAGE_REGISTER.
 * bit 2 = Reserved, must be 0.
 * bit 1 = Layer 2 screen visible on the display.
 * bit 0 = Enable the paged-in layer 2 screen (main or shadow) for writing.
 */

#define LAYER2_PORT 0x123B
#define BANK_PORT 0x7FFD
#define EXT_BANK_PORT 0xDFFD

#define BANK_SYS_VAR 0x5B5C
#define ROM_FONT_ADDRESS 0x3D00

#define LAYER2_VISIBLE_MASK 0x02
#define LAYER2_SHADOW_SCREEN_MASK 0x08
#define LAYER2_WRITE_ENABLE_MASK 0x01
#define LAYER2_SCREEN_SECTION_MASK 0x03
#define LAYER2_SCREEN_SECTION_SHIFT 6

#define LAYER_PRIORITIES_MASK 0x07
#define LAYER_PRIORITIES_SHIFT 2

#define SCREEN_ADDRESS(screen) ((((screen) != NULL) && ((screen)->screen_type == OFF_SCREEN)) ? (uint8_t *) 0xC000 : (uint8_t *) 0)

__sfr __banked __at REGISTER_NUMBER_PORT IO_REGISTER_NUMBER_PORT;
__sfr __banked __at REGISTER_VALUE_PORT IO_REGISTER_VALUE_PORT;
__sfr __banked __at LAYER2_PORT IO_LAYER2_PORT;
__sfr __banked __at BANK_PORT IO_BANK_PORT;
__sfr __banked __at EXT_BANK_PORT IO_EXT_BANK_PORT;

#endif
