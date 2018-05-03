/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_set_shadow_screen_ram_bank() in zxnext_layer2.h.
 ******************************************************************************/

#include <arch/zxn.h>
#include <stdint.h>

#include "zxnext_layer2.h"

void layer2_set_shadow_screen_ram_bank(uint8_t bank)
{
    IO_NEXTREG_REG = REG_LAYER_2_SHADOW_RAM_PAGE;
    IO_NEXTREG_DAT = bank;
}
