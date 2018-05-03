/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_get_main_screen_ram_bank() in zxnext_layer2.h.
 ******************************************************************************/

#include <arch/zxn.h>
#include <stdint.h>

#include "zxnext_layer2.h"

uint8_t layer2_get_main_screen_ram_bank(void)
{
    IO_NEXTREG_REG = REG_LAYER_2_RAM_PAGE;
    return IO_NEXTREG_DAT;
}
