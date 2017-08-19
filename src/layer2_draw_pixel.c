/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_draw_pixel() in zxnext_layer2.h.
 ******************************************************************************/

#include <z80.h>
#include <stdint.h>

#include "zxnext_layer2.h"
#include "layer2_defs.h"
#include "layer2_common.h"

void layer2_draw_pixel(uint8_t x,
                       uint8_t y,
                       uint8_t color,
                       off_screen_buffer_t *off_screen_buffer)
{
    if (y > 191)
    {
        return;
    }

    init_switch_screen(off_screen_buffer);

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

    end_switch_screen(off_screen_buffer);
}
