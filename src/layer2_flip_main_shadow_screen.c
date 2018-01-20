/*******************************************************************************
 * Stefan Bylund 2018
 *
 * Implementation of layer2_flip_main_shadow_screen() in zxnext_layer2.h.
 ******************************************************************************/

#include <stdint.h>

#include "zxnext_layer2.h"
#include "layer2_defs.h"

void layer2_flip_main_shadow_screen(void)
{
    uint8_t main_screen_ram_bank;
    uint8_t shadow_screen_ram_bank;

    IO_REGISTER_NUMBER_PORT = LAYER2_RAM_PAGE_REGISTER;
    main_screen_ram_bank = IO_REGISTER_VALUE_PORT;

    IO_REGISTER_NUMBER_PORT = LAYER2_SHADOW_RAM_PAGE_REGISTER;
    shadow_screen_ram_bank = IO_REGISTER_VALUE_PORT;

    IO_REGISTER_NUMBER_PORT = LAYER2_RAM_PAGE_REGISTER;
    IO_REGISTER_VALUE_PORT = shadow_screen_ram_bank;

    IO_REGISTER_NUMBER_PORT = LAYER2_SHADOW_RAM_PAGE_REGISTER;
    IO_REGISTER_VALUE_PORT = main_screen_ram_bank;
}
