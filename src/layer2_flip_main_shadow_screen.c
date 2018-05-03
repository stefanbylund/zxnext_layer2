/*******************************************************************************
 * Stefan Bylund 2018
 *
 * Implementation of layer2_flip_main_shadow_screen() in zxnext_layer2.h.
 ******************************************************************************/

#include <arch/zxn.h>
#include <stdint.h>

#include "zxnext_layer2.h"

void layer2_flip_main_shadow_screen(void)
{
    uint8_t main_screen_ram_bank;
    uint8_t shadow_screen_ram_bank;

    IO_NEXTREG_REG = REG_LAYER_2_RAM_PAGE;
    main_screen_ram_bank = IO_NEXTREG_DAT;

    IO_NEXTREG_REG = REG_LAYER_2_SHADOW_RAM_PAGE;
    shadow_screen_ram_bank = IO_NEXTREG_DAT;

    IO_NEXTREG_REG = REG_LAYER_2_RAM_PAGE;
    IO_NEXTREG_DAT = shadow_screen_ram_bank;

    IO_NEXTREG_REG = REG_LAYER_2_SHADOW_RAM_PAGE;
    IO_NEXTREG_DAT = main_screen_ram_bank;
}
