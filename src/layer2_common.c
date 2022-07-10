/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Common functions and variables used by the implementation of zxnext_layer2.h.
 ******************************************************************************/

#include <arch/zxn.h>
#include <z80.h>
#include <intrinsic.h>
#include <stddef.h>
#include <stdint.h>

#include "zxnext_layer2.h"
#include "layer2_common.h"

uint8_t buf_256[256];

void init_switch_screen(layer2_screen_t *screen)
{
    if ((screen != NULL) && (screen->screen_type == OFF_SCREEN))
    {
        screen->tmp = z80_bpeek(__SYSVAR_BANKM);
    }
}

void switch_top_screen_section(layer2_screen_t *screen)
{
    if ((screen == NULL) || (screen->screen_type == MAIN_SCREEN))
    {
        layer2_configure(true, true, false, LAYER2_SCREEN_TOP);
    }
    else if (screen->screen_type == SHADOW_SCREEN)
    {
        layer2_configure(true, true, true, LAYER2_SCREEN_TOP);
    }
    else
    {
        switch_ram_bank(screen->top_bank);
    }
}

void switch_middle_screen_section(layer2_screen_t *screen)
{
    if ((screen == NULL) || (screen->screen_type == MAIN_SCREEN))
    {
        layer2_configure(true, true, false, LAYER2_SCREEN_MIDDLE);
    }
    else if (screen->screen_type == SHADOW_SCREEN)
    {
        layer2_configure(true, true, true, LAYER2_SCREEN_MIDDLE);
    }
    else
    {
        switch_ram_bank(screen->middle_bank);
    }
}

void switch_bottom_screen_section(layer2_screen_t *screen)
{
    if ((screen == NULL) || (screen->screen_type == MAIN_SCREEN))
    {
        layer2_configure(true, true, false, LAYER2_SCREEN_BOTTOM);
    }
    else if (screen->screen_type == SHADOW_SCREEN)
    {
        layer2_configure(true, true, true, LAYER2_SCREEN_BOTTOM);
    }
    else
    {
        switch_ram_bank(screen->bottom_bank);
    }
}

void end_switch_screen(layer2_screen_t *screen)
{
    if ((screen == NULL) || (screen->screen_type != OFF_SCREEN))
    {
        layer2_configure(true, false, false, 0);
    }
    else
    {
        switch_ram_bank(screen->tmp);
    }
}

void switch_ram_bank(uint8_t bank)
{
    uint8_t page = bank << 1;

    intrinsic_di();
    z80_bpoke(__SYSVAR_BANKM, ((z80_bpeek(__SYSVAR_BANKM) & 0xF8) | (bank & 0x07)));
    ZXN_WRITE_MMU6(page);
    ZXN_WRITE_MMU7(page + 1);
    intrinsic_ei();
}
