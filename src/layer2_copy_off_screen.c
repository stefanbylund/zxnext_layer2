/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_copy_off_screen() in zxnext_layer2.h.
 ******************************************************************************/

#include <z80.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "zxnext_layer2.h"
#include "layer2_defs.h"
#include "layer2_common.h"

void layer2_copy_off_screen(off_screen_buffer_t *off_screen_buffer)
{
    if (off_screen_buffer == NULL)
    {
        return;
    }

    off_screen_buffer->tmp = z80_bpeek(BANK_SYS_VAR);

    layer2_configure(true, true, false, LAYER2_SCREEN_TOP);
    switch_ram_bank(off_screen_buffer->top_bank);
    memcpy((void *) 0, (void *) 0xC000, 16384);

    layer2_configure(true, true, false, LAYER2_SCREEN_MIDDLE);
    switch_ram_bank(off_screen_buffer->middle_bank);
    memcpy((void *) 0, (void *) 0xC000, 16384);

    layer2_configure(true, true, false, LAYER2_SCREEN_BOTTOM);
    switch_ram_bank(off_screen_buffer->bottom_bank);
    memcpy((void *) 0, (void *) 0xC000, 16384);

    layer2_configure(true, false, false, 0);
    switch_ram_bank(off_screen_buffer->tmp);
}
