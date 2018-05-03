/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_reset_palette() in zxnext_layer2.h.
 ******************************************************************************/

#include <arch/zxn.h>
#include <stdint.h>

#include "zxnext_layer2.h"

void layer2_reset_palette(void)
{
    uint16_t i;

    IO_NEXTREG_REG = REG_PALETTE_INDEX;
    IO_NEXTREG_DAT = 0;

    IO_NEXTREG_REG = REG_PALETTE_VALUE_8;
    for (i = 0; i < 256; i++)
    {
        IO_NEXTREG_DAT = (uint8_t) i;
    }
}
