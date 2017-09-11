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
                                     layer2_screen_t *screen);

void layer2_fill_rect(uint8_t x,
                      uint8_t y,
                      uint16_t width,
                      uint8_t height,
                      uint8_t color,
                      layer2_screen_t *screen)
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

    init_switch_screen(screen);

    if (y < 64)
    {
        // top || top + middle || top + middle + bottom
        if (y + height - 1 < 64)
        {
            // top
            switch_top_screen_section(screen);
            layer2_fill_rect_section(x, y, width, height, screen);
        }
        else if (y + height - 1 < 128)
        {
            // top
            switch_top_screen_section(screen);
            layer2_fill_rect_section(x, y, width, 64 - y, screen);

            // middle
            switch_middle_screen_section(screen);
            layer2_fill_rect_section(x, 0, width, height - (64 - y), screen);
        }
        else
        {
            // top
            switch_top_screen_section(screen);
            layer2_fill_rect_section(x, y, width, 64 - y, screen);

            // middle
            switch_middle_screen_section(screen);
            layer2_fill_rect_section(x, 0, width, 64, screen);

            // bottom
            switch_bottom_screen_section(screen);
            layer2_fill_rect_section(x, 0, width, height - (128 - y), screen);
        }
    }
    else if (y < 128)
    {
        // middle || middle + bottom
        if (y + height - 1 < 128)
        {
            // middle
            switch_middle_screen_section(screen);
            layer2_fill_rect_section(x, y - 64, width, height, screen);
        }
        else
        {
            // middle
            switch_middle_screen_section(screen);
            layer2_fill_rect_section(x, y - 64, width, 128 - y, screen);

            // bottom
            switch_bottom_screen_section(screen);
            layer2_fill_rect_section(x, 0, width, height - (128 - y), screen);
        }
    }
    else
    {
        // bottom
        switch_bottom_screen_section(screen);
        layer2_fill_rect_section(x, y - 128, width, height, screen);
    }

    end_switch_screen(screen);
}

static void layer2_fill_rect_section(uint8_t x,
                                     uint8_t y,
                                     uint16_t width,
                                     uint8_t height,
                                     layer2_screen_t *screen)
{
    uint8_t *dest = SCREEN_ADDRESS(screen) + x + (y << 8);

    // buf_256 is set by layer2_fill_rect().

    while (height--)
    {
        memcpy(dest, buf_256, width);
        dest += 256;
    }
}
