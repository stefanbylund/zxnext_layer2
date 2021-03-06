/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_blit() in zxnext_layer2.h.
 ******************************************************************************/

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "zxnext_layer2.h"
#include "layer2_defs.h"
#include "layer2_common.h"

static void layer2_blit_section(uint8_t x,
                                uint8_t y,
                                const uint8_t *source,
                                uint16_t width,
                                uint8_t height,
                                layer2_screen_t *screen);

void layer2_blit(uint8_t x,
                 uint8_t y,
                 const uint8_t *source,
                 uint16_t width,
                 uint8_t height,
                 layer2_screen_t *screen)
{
    if ((y > 191) || (source == NULL) || (width == 0) || (height == 0))
    {
        return;
    }

    // Width clip handling is done in layer2_blit_section().

    if (y + height - 1 > 191)
    {
        height = 191 - y + 1;
    }

    init_switch_screen(screen);

    if (y < 64)
    {
        // top || top + middle || top + middle + bottom
        if (y + height - 1 < 64)
        {
            // top
            switch_top_screen_section(screen);
            layer2_blit_section(x, y, source, width, height, screen);
        }
        else if (y + height - 1 < 128)
        {
            // top
            switch_top_screen_section(screen);
            layer2_blit_section(x, y, source, width, 64 - y, screen);

            // middle
            switch_middle_screen_section(screen);
            layer2_blit_section(x, 0, source + width * (64 - y), width, height - (64 - y), screen);
        }
        else
        {
            // top
            switch_top_screen_section(screen);
            layer2_blit_section(x, y, source, width, 64 - y, screen);

            // middle
            switch_middle_screen_section(screen);
            layer2_blit_section(x, 0, source + width * (64 - y), width, 64, screen);

            // bottom
            switch_bottom_screen_section(screen);
            layer2_blit_section(x, 0, source + width * (128 - y), width, height - (128 - y), screen);
        }
    }
    else if (y < 128)
    {
        // middle || middle + bottom
        if (y + height - 1 < 128)
        {
            // middle
            switch_middle_screen_section(screen);
            layer2_blit_section(x, y - 64, source, width, height, screen);
        }
        else
        {
            // middle
            switch_middle_screen_section(screen);
            layer2_blit_section(x, y - 64, source, width, 128 - y, screen);

            // bottom
            switch_bottom_screen_section(screen);
            layer2_blit_section(x, 0, source + width * (128 - y), width, height - (128 - y), screen);
        }
    }
    else
    {
        // bottom
        switch_bottom_screen_section(screen);
        layer2_blit_section(x, y - 128, source, width, height, screen);
    }

    end_switch_screen(screen);
}

static void layer2_blit_section(uint8_t x,
                                uint8_t y,
                                const uint8_t *source,
                                uint16_t width,
                                uint8_t height,
                                layer2_screen_t *screen)
{
    uint8_t *dest = SCREEN_ADDRESS(screen) + x + (y << 8);
    uint16_t clip_width = width;

    if (x + width - 1 > 255)
    {
        clip_width = 255 - x + 1;
    }

    if ((x == 0) && (width == 256) && (width == clip_width))
    {
        memcpy(dest, source, width * height);
    }
    else
    {
        while (height--)
        {
            memcpy(dest, source, clip_width);
            dest += 256;
            source += width;
        }
    }
}
