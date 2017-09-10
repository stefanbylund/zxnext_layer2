/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_get_main_screen_ram_bank() in zxnext_layer2.h.
 ******************************************************************************/

#include <stdint.h>

#include "zxnext_layer2.h"
#include "layer2_defs.h"

uint8_t layer2_get_main_screen_ram_bank(void)
{
    IO_REGISTER_NUMBER_PORT = LAYER2_RAM_PAGE_REGISTER;
    return IO_REGISTER_VALUE_PORT;
}
