/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_clear_screen() in zxnext_layer2.h.
 ******************************************************************************/

#include <stdint.h>
#include <string.h>

#include "zxnext_layer2.h"
#include "layer2_defs.h"
#include "layer2_common.h"

static void layer2_clear_screen_section(off_screen_buffer_t *off_screen_buffer);

void layer2_clear_screen(uint8_t color, off_screen_buffer_t *off_screen_buffer)
{
    // Used in layer2_clear_screen_section().
    memset(buf_256, color, 256);

    init_switch_screen(off_screen_buffer);

    switch_top_screen_section(off_screen_buffer);
    layer2_clear_screen_section(off_screen_buffer);

    switch_middle_screen_section(off_screen_buffer);
    layer2_clear_screen_section(off_screen_buffer);

    switch_bottom_screen_section(off_screen_buffer);
    layer2_clear_screen_section(off_screen_buffer);

    end_switch_screen(off_screen_buffer);
}

static void layer2_clear_screen_section(off_screen_buffer_t *off_screen_buffer)
{
    uint8_t height = 64;
    uint8_t *dest = SCREEN_ADDRESS(off_screen_buffer);

    // buf_256 is set by layer2_clear_screen().

    while (height--)
    {
        // TODO: Doesn't work with sdcc_ix.
        //intrinsic_ldi(dest, buf_256, 256);
        memcpy(dest, buf_256, 256);
        dest += 256;
    }
}
