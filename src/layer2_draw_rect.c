/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_draw_rect() in zxnext_layer2.h.
 ******************************************************************************/

#include <z80.h>
#include <stdbool.h>
#include <string.h>

#include "zxnext_layer2.h"
#include "layer2_defs.h"
#include "layer2_common.h"

static void layer2_draw_horizontal_line(uint8_t x,
                                        uint8_t y,
                                        uint16_t width,
                                        layer2_screen_t *screen);

static void layer2_draw_vertical_line(uint8_t x,
                                      uint8_t y,
                                      uint16_t height,
                                      uint8_t color,
                                      layer2_screen_t *screen);

static void layer2_draw_vertical_line_section(uint8_t x,
                                              uint8_t y,
                                              uint16_t height,
                                              uint8_t color,
                                              layer2_screen_t *screen);

void layer2_draw_rect(uint8_t x,
                      uint8_t y,
                      uint16_t width,
                      uint8_t height,
                      uint8_t color,
                      layer2_screen_t *screen)
{
    bool clip_width = false;
    bool clip_height = false;

    if ((y > 191) || (width == 0) || (height == 0))
    {
        return;
    }

    if (x + width - 1 > 255)
    {
        width = 255 - x + 1;
        clip_width = true;
    }

    if (y + height - 1 > 191)
    {
        height = 191 - y + 1;
        clip_height = true;
    }

    // Used in layer2_draw_horizontal_line().
    memset(buf_256, color, width);

    init_switch_screen(screen);

    // top
    layer2_draw_horizontal_line(x, y, width, screen);

    // left
    layer2_draw_vertical_line(x, y, height, color, screen);

    // bottom
    if (!clip_height)
    {
        layer2_draw_horizontal_line(x, y + height - 1, width, screen);
    }

    // right
    if (!clip_width)
    {
        layer2_draw_vertical_line(x + width - 1, y, height, color, screen);
    }

    end_switch_screen(screen);
}

static void layer2_draw_horizontal_line(uint8_t x,
                                        uint8_t y,
                                        uint16_t width,
                                        layer2_screen_t *screen)
{
    if (y < 64)
    {
        // top
        switch_top_screen_section(screen);
    }
    else if (y < 128)
    {
        // middle
        switch_middle_screen_section(screen);
        y -= 64;
    }
    else
    {
        // bottom
        switch_bottom_screen_section(screen);
        y -= 128;
    }

    // buf_256 is set by layer2_draw_rect().

    memcpy(SCREEN_ADDRESS(screen) + x + (y << 8), buf_256, width);
}

static void layer2_draw_vertical_line(uint8_t x,
                                      uint8_t y,
                                      uint16_t height,
                                      uint8_t color,
                                      layer2_screen_t *screen)
{
    if (y < 64)
    {
        // top || top + middle || top + middle + bottom
        if (y + height - 1 < 64)
        {
            // top
            switch_top_screen_section(screen);
            layer2_draw_vertical_line_section(x, y, height, color, screen);
        }
        else if (y + height - 1 < 128)
        {
            // top
            switch_top_screen_section(screen);
            layer2_draw_vertical_line_section(x, y, 64 - y, color, screen);

            // middle
            switch_middle_screen_section(screen);
            layer2_draw_vertical_line_section(x, 0, height - (64 - y), color, screen);
        }
        else
        {
            // top
            switch_top_screen_section(screen);
            layer2_draw_vertical_line_section(x, y, 64 - y, color, screen);

            // middle
            switch_middle_screen_section(screen);
            layer2_draw_vertical_line_section(x, 0, 64, color, screen);

            // bottom
            switch_bottom_screen_section(screen);
            layer2_draw_vertical_line_section(x, 0, height - (128 - y), color, screen);
        }
    }
    else if (y < 128)
    {
        // middle || middle + bottom
        if (y + height - 1 < 128)
        {
            // middle
            switch_middle_screen_section(screen);
            layer2_draw_vertical_line_section(x, y - 64, height, color, screen);
        }
        else
        {
            // middle
            switch_middle_screen_section(screen);
            layer2_draw_vertical_line_section(x, y - 64, 128 - y, color, screen);

            // bottom
            switch_bottom_screen_section(screen);
            layer2_draw_vertical_line_section(x, 0, height - (128 - y), color, screen);
        }
    }
    else
    {
        // bottom
        switch_bottom_screen_section(screen);
        layer2_draw_vertical_line_section(x, y - 128, height, color, screen);
    }
}

static void layer2_draw_vertical_line_section(uint8_t x,
                                              uint8_t y,
                                              uint16_t height,
                                              uint8_t color,
                                              layer2_screen_t *screen)
{
    uint8_t *dest = SCREEN_ADDRESS(screen) + x + (y << 8);

    while (height--)
    {
        z80_bpoke(dest, color);
        dest += 256;
    }
}
