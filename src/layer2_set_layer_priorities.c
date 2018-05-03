/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_set_layer_priorities() in zxnext_layer2.h.
 ******************************************************************************/

#include <arch/zxn.h>
#include <stdint.h>

#include "zxnext_layer2.h"

#define LAYER_PRIORITIES_MASK 0x07
#define LAYER_PRIORITIES_SHIFT 2

void layer2_set_layer_priorities(uint8_t layer_priorities)
{
    IO_NEXTREG_REG = REG_SPRITE_LAYER_SYSTEM;
    IO_NEXTREG_DAT = (IO_NEXTREG_DAT & 0xE3) |
        ((layer_priorities & LAYER_PRIORITIES_MASK) << LAYER_PRIORITIES_SHIFT);
}
