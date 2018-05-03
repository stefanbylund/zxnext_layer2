/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_set_palette() in zxnext_layer2.h.
 ******************************************************************************/

#include <arch/zxn.h>
#include <stdint.h>

#include "zxnext_layer2.h"

void layer2_set_palette(const uint16_t *colors, uint16_t length, uint8_t palette_index)
{
    uint8_t *color_bytes = (uint8_t *) colors;
    uint16_t i;

    if ((colors == NULL) || (length == 0))
    {
        return;
    }

    if (palette_index + length > 256)
    {
        length = 256 - palette_index;
    }

    IO_NEXTREG_REG = REG_PALETTE_INDEX;
    IO_NEXTREG_DAT = palette_index;

    IO_NEXTREG_REG = REG_PALETTE_VALUE_16;
    for (i = 0; i < (length << 1); i++)
    {
        IO_NEXTREG_DAT = color_bytes[i];
    }
}
