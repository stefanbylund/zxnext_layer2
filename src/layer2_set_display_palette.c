/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_set_display_palette() in zxnext_layer2.h.
 ******************************************************************************/

#include <arch/zxn.h>
#include <stdbool.h>

#include "zxnext_layer2.h"

void layer2_set_display_palette(bool first_palette)
{
    IO_NEXTREG_REG = REG_PALETTE_CONTROL;
    IO_NEXTREG_DAT = (IO_NEXTREG_DAT & 0xFB) | (first_palette ? 0x00 : 0x04);
}
