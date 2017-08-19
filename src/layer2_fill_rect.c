/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_fill_rect() in zxnext_layer2.h.
 ******************************************************************************/

#include <stdint.h>
#include <string.h>

#include "zxnext_layer2.h"
#include "layer2_defs.h"
#include "layer2_common.h"

static void layer2_fill_rect_section(uint8_t x,
                                     uint8_t y,
                                     uint16_t width,
                                     uint8_t height,
                                     off_screen_buffer_t *off_screen_buffer);

void layer2_fill_rect(uint8_t x,
                      uint8_t y,
                      uint16_t width,
                      uint8_t height,
                      uint8_t color,
                      off_screen_buffer_t *off_screen_buffer)
{
    if ((y > 191) || (width == 0) || (height == 0))
    {
        return;
    }

    if (x + width - 1 > 255)
    {
        width = 255 - x + 1;
    }

    if (y + height - 1 > 191)
    {
        height = 191 - y + 1;
    }

    // Used in layer2_fill_rect_section().
    memset(buf_256, color, width);

    init_switch_screen(off_screen_buffer);

    if (y < 64)
    {
        // top || top + middle || top + middle + bottom
        if (y + height - 1 < 64)
        {
            // top
            switch_top_screen_section(off_screen_buffer);
            layer2_fill_rect_section(x, y, width, height, off_screen_buffer);
        }
        else if (y + height - 1 < 128)
        {
            // top
            switch_top_screen_section(off_screen_buffer);
            layer2_fill_rect_section(x, y, width, 64 - y, off_screen_buffer);

            // middle
            switch_middle_screen_section(off_screen_buffer);
            layer2_fill_rect_section(x, 0, width, height - (64 - y), off_screen_buffer);
        }
        else
        {
            // top
            switch_top_screen_section(off_screen_buffer);
            layer2_fill_rect_section(x, y, width, 64 - y, off_screen_buffer);

            // middle
            switch_middle_screen_section(off_screen_buffer);
            layer2_fill_rect_section(x, 0, width, 64, off_screen_buffer);

            // bottom
            switch_bottom_screen_section(off_screen_buffer);
            layer2_fill_rect_section(x, 0, width, height - (128 - y), off_screen_buffer);
        }
    }
    else if (y < 128)
    {
        // middle || middle + bottom
        if (y + height - 1 < 128)
        {
            // middle
            switch_middle_screen_section(off_screen_buffer);
            layer2_fill_rect_section(x, y - 64, width, height, off_screen_buffer);
        }
        else
        {
            // middle
            switch_middle_screen_section(off_screen_buffer);
            layer2_fill_rect_section(x, y - 64, width, 128 - y, off_screen_buffer);

            // bottom
            switch_bottom_screen_section(off_screen_buffer);
            layer2_fill_rect_section(x, 0, width, height - (128 - y), off_screen_buffer);
        }
    }
    else
    {
        // bottom
        switch_bottom_screen_section(off_screen_buffer);
        layer2_fill_rect_section(x, y - 128, width, height, off_screen_buffer);
    }

    end_switch_screen(off_screen_buffer);
}

static void layer2_fill_rect_section(uint8_t x,
                                     uint8_t y,
                                     uint16_t width,
                                     uint8_t height,
                                     off_screen_buffer_t *off_screen_buffer)
{
    uint8_t *dest = SCREEN_ADDRESS(off_screen_buffer) + x + (y << 8);

    // buf_256 is set by layer2_fill_rect().

    while (height--)
    {
        memcpy(dest, buf_256, width);
        dest += 256;
    }
}
