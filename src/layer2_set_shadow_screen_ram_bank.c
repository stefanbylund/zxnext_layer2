/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_set_shadow_screen_ram_bank() in zxnext_layer2.h.
 ******************************************************************************/

#include <stdint.h>

#include "zxnext_layer2.h"
#include "layer2_defs.h"

void layer2_set_shadow_screen_ram_bank(uint8_t bank)
{
    IO_REGISTER_NUMBER_PORT = LAYER2_SHADOW_RAM_PAGE_REGISTER;
    IO_REGISTER_VALUE_PORT = bank;
}
