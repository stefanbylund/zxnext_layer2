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

#define LAYER2_PORT 0x123B
#define BANK_PORT 0x7FFD

#define BANK_SYS_VAR 0x5B5C
#define ROM_FONT_ADDRESS 0x3D00

#define LAYER2_VISIBLE_MASK 0x02
#define LAYER2_BEHIND_ULA_SCREEN_MASK 0x10
#define LAYER2_WRITE_ENABLE_MASK 0x01
#define LAYER2_ULA_TRANSPARENCY_COLOR_MASK 0x0F
#define LAYER2_SCREEN_SECTION_MASK 0x03
#define LAYER2_SCREEN_SECTION_SHIFT 6

#define SCREEN_ADDRESS(off_screen_buffer) (((off_screen_buffer) != NULL) ? (uint8_t *) 0xC000 : (uint8_t *) 0)

__sfr __banked __at REGISTER_NUMBER_PORT IO_REGISTER_NUMBER_PORT;
__sfr __banked __at REGISTER_VALUE_PORT IO_REGISTER_VALUE_PORT;
__sfr __banked __at LAYER2_PORT IO_LAYER2_PORT;
__sfr __banked __at BANK_PORT IO_BANK_PORT;

#endif
