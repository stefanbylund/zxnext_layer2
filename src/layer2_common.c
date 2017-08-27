/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Common functions and variables used by the implementation of zxnext_layer2.h.
 ******************************************************************************/

#include <z80.h>
#include <intrinsic.h>
#include <stddef.h>
#include <stdint.h>

#include "zxnext_layer2.h"
#include "layer2_defs.h"
#include "layer2_common.h"

uint8_t buf_256[256];

void init_switch_screen(off_screen_buffer_t *off_screen_buffer)
{
    if (off_screen_buffer != NULL)
    {
        off_screen_buffer->tmp = z80_bpeek(BANK_SYS_VAR);
    }
}

void switch_top_screen_section(off_screen_buffer_t *off_screen_buffer)
{
    if (off_screen_buffer == NULL)
    {
        layer2_configure(true, false, true, LAYER2_SCREEN_TOP);
    }
    else
    {
        switch_ram_bank(off_screen_buffer->top_bank);
    }
}

void switch_middle_screen_section(off_screen_buffer_t *off_screen_buffer)
{
    if (off_screen_buffer == NULL)
    {
        layer2_configure(true, false, true, LAYER2_SCREEN_MIDDLE);
    }
    else
    {
        switch_ram_bank(off_screen_buffer->middle_bank);
    }
}

void switch_bottom_screen_section(off_screen_buffer_t *off_screen_buffer)
{
    if (off_screen_buffer == NULL)
    {
        layer2_configure(true, false, true, LAYER2_SCREEN_BOTTOM);
    }
    else
    {
        switch_ram_bank(off_screen_buffer->bottom_bank);
    }
}

void end_switch_screen(off_screen_buffer_t *off_screen_buffer)
{
    if (off_screen_buffer == NULL)
    {
        layer2_configure(true, false, false, 0);
    }
    else
    {
        switch_ram_bank(off_screen_buffer->tmp);
    }
}

void switch_ram_bank(uint8_t bank)
{
    uint8_t old_value = z80_bpeek(BANK_SYS_VAR);
    uint8_t new_value = (old_value & 0xF8) | (bank & 0x07);
    uint8_t ext_value = (bank & 0x38) >> 3;

    intrinsic_di();
    z80_bpoke(BANK_SYS_VAR, new_value);
    IO_BANK_PORT = new_value;
    IO_EXT_BANK_PORT = ext_value;
    intrinsic_ei();
}

void switch_rom_bank(uint8_t bank)
{
    uint8_t old_value = z80_bpeek(BANK_SYS_VAR);
    uint8_t new_value = (old_value & 0xEF) | ((bank & 0x01) << 4);

    intrinsic_di();
    z80_bpoke(BANK_SYS_VAR, new_value);
    IO_BANK_PORT = new_value;
    intrinsic_ei();
}
