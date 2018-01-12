/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_reset_palette() in zxnext_layer2.h.
 ******************************************************************************/

#include <stdint.h>

#include "zxnext_layer2.h"
#include "layer2_defs.h"

void layer2_reset_palette(void)
{
    uint16_t i;

    IO_REGISTER_NUMBER_PORT = PALETTE_INDEX_REGISTER;
    IO_REGISTER_VALUE_PORT = 0;

    IO_REGISTER_NUMBER_PORT = PALETTE_VALUE_8BIT_REGISTER;
    for (i = 0; i < 256; i++)
    {
        IO_REGISTER_VALUE_PORT = (uint8_t) i;
    }
}
