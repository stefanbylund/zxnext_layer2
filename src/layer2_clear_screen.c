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

static void layer2_clear_screen_section(layer2_screen_t *screen);

void layer2_clear_screen(uint8_t color, layer2_screen_t *screen)
{
    // Used in layer2_clear_screen_section().
    memset(buf_256, color, 256);

    init_switch_screen(screen);

    switch_top_screen_section(screen);
    layer2_clear_screen_section(screen);

    switch_middle_screen_section(screen);
    layer2_clear_screen_section(screen);

    switch_bottom_screen_section(screen);
    layer2_clear_screen_section(screen);

    end_switch_screen(screen);
}

static void layer2_clear_screen_section(layer2_screen_t *screen)
{
    uint8_t height = 64;
    uint8_t *dest = SCREEN_ADDRESS(screen);

    // buf_256 is set by layer2_clear_screen().

    while (height--)
    {
        // TODO: Doesn't work with sdcc_ix.
        //intrinsic_ldi(dest, buf_256, 256);
        memcpy(dest, buf_256, 256);
        dest += 256;
    }
}
