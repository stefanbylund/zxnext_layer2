/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of helper function layer2_draw_pixel_fast().
 *
 * The caller of this function is required to validate its arguments and make
 * sure it is called within init_switch_screen() and end_switch_screen().
 ******************************************************************************/

#include <z80.h>
#include <stdint.h>

#include "zxnext_layer2.h"
#include "layer2_defs.h"
#include "layer2_common.h"
#include "layer2_draw_pixel_fast.h"

void layer2_draw_pixel_fast(uint8_t x,
                            uint8_t y,
                            uint8_t color,
                            off_screen_buffer_t *off_screen_buffer)
{
    if (y < 64)
    {
        // top
        switch_top_screen_section(off_screen_buffer);
    }
    else if (y < 128)
    {
        // middle
        switch_middle_screen_section(off_screen_buffer);
        y -= 64;
    }
    else
    {
        // bottom
        switch_bottom_screen_section(off_screen_buffer);
        y -= 128;
    }

    z80_bpoke(SCREEN_ADDRESS(off_screen_buffer) + x + (y << 8), color);
}
