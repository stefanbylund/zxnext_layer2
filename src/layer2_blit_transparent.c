/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_blit_transparent() in zxnext_layer2.h.
 ******************************************************************************/

#include <z80.h>
#include <stddef.h>
#include <stdint.h>

#include "zxnext_layer2.h"
#include "layer2_defs.h"
#include "layer2_common.h"

static void layer2_blit_transparent_section(uint8_t x,
                                            uint8_t y,
                                            const uint8_t *source,
                                            uint16_t width,
                                            uint8_t height,
                                            off_screen_buffer_t *off_screen_buffer);

void layer2_blit_transparent(uint8_t x,
                             uint8_t y,
                             const uint8_t *source,
                             uint16_t width,
                             uint8_t height,
                             off_screen_buffer_t *off_screen_buffer)
{
    if ((y > 191) || (source == NULL) || (width == 0) || (height == 0))
    {
        return;
    }

    // Width clip handling is done in layer2_blit_transparent_section().

    if (y + height - 1 > 191)
    {
        height = 191 - y + 1;
    }

    init_switch_screen(off_screen_buffer);

    if (y < 64)
    {
        // top || top + middle || top + middle + bottom
        if (y + height - 1 < 64)
        {
            // top
            switch_top_screen_section(off_screen_buffer);
            layer2_blit_transparent_section(x, y, source, width, height, off_screen_buffer);
        }
        else if (y + height - 1 < 128)
        {
            // top
            switch_top_screen_section(off_screen_buffer);
            layer2_blit_transparent_section(x, y, source, width, 64 - y, off_screen_buffer);

            // middle
            switch_middle_screen_section(off_screen_buffer);
            layer2_blit_transparent_section(x, 0, source + width * (64 - y), width, height - (64 - y), off_screen_buffer);
        }
        else
        {
            // top
            switch_top_screen_section(off_screen_buffer);
            layer2_blit_transparent_section(x, y, source, width, 64 - y, off_screen_buffer);

            // middle
            switch_middle_screen_section(off_screen_buffer);
            layer2_blit_transparent_section(x, 0, source + width * (64 - y), width, 64, off_screen_buffer);

            // bottom
            switch_bottom_screen_section(off_screen_buffer);
            layer2_blit_transparent_section(x, 0, source + width * (128 - y), width, height - (128 - y), off_screen_buffer);
        }
    }
    else if (y < 128)
    {
        // middle || middle + bottom
        if (y + height - 1 < 128)
        {
            // middle
            switch_middle_screen_section(off_screen_buffer);
            layer2_blit_transparent_section(x, y - 64, source, width, height, off_screen_buffer);
        }
        else
        {
            // middle
            switch_middle_screen_section(off_screen_buffer);
            layer2_blit_transparent_section(x, y - 64, source, width, 128 - y, off_screen_buffer);

            // bottom
            switch_bottom_screen_section(off_screen_buffer);
            layer2_blit_transparent_section(x, 0, source + width * (128 - y), width, height - (128 - y), off_screen_buffer);
        }
    }
    else
    {
        // bottom
        switch_bottom_screen_section(off_screen_buffer);
        layer2_blit_transparent_section(x, y - 128, source, width, height, off_screen_buffer);
    }

    end_switch_screen(off_screen_buffer);
}

static void layer2_blit_transparent_section(uint8_t x,
                                            uint8_t y,
                                            const uint8_t *source,
                                            uint16_t width,
                                            uint8_t height,
                                            off_screen_buffer_t *off_screen_buffer)
{
    uint8_t *dest = SCREEN_ADDRESS(off_screen_buffer) + x + (y << 8);
    uint16_t clip_width = width;

    if (x + width - 1 > 255)
    {
        clip_width = 255 - x + 1;
    }

    while (height--)
    {
        uint16_t i;
        for (i = 0; i < clip_width; i++)
        {
            if (source[i] != 0xE3)
            {
                z80_bpoke(dest + i, source[i]);
            }
        }

        dest += 256;
        source += width;
    }
}
