/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_blit_off_screen_column() in zxnext_layer2.h.
 ******************************************************************************/

#include <z80.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "zxnext_layer2.h"
#include "layer2_defs.h"
#include "layer2_common.h"

static void layer2_blit_off_screen_column_section(uint8_t dest_x, uint8_t source_x);

void layer2_blit_off_screen_column(uint8_t dest_x, off_screen_buffer_t *source, uint8_t source_x)
{
    if (source == NULL)
    {
        return;
    }

    source->tmp = z80_bpeek(BANK_SYS_VAR);

    // top
    layer2_configure(true, true, false, LAYER2_SCREEN_TOP);
    switch_ram_bank(source->top_bank);
    layer2_blit_off_screen_column_section(dest_x, source_x);

    // middle
    layer2_configure(true, true, false, LAYER2_SCREEN_MIDDLE);
    switch_ram_bank(source->middle_bank);
    layer2_blit_off_screen_column_section(dest_x, source_x);

    // bottom
    layer2_configure(true, true, false, LAYER2_SCREEN_BOTTOM);
    switch_ram_bank(source->bottom_bank);
    layer2_blit_off_screen_column_section(dest_x, source_x);

    layer2_configure(true, false, false, 0);
    switch_ram_bank(source->tmp);
}

static void layer2_blit_off_screen_column_section(uint8_t dest_x, uint8_t source_x)
{
    uint8_t *dest = (uint8_t *) dest_x;
    uint8_t *source = (uint8_t *) (0xC000 + source_x);
    uint8_t height = 64;

    while (height--)
    {
        z80_bpoke(dest, *source);
        dest += 256;
        source += 256;
    }
}
