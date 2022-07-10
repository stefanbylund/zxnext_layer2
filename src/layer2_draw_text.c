/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_draw_text() and layer2_set_font() in zxnext_layer2.h.
 ******************************************************************************/

#include <arch/zxn.h>
#include <z80.h>
#include <stddef.h>

#include "zxnext_layer2.h"
#include "layer2_defs.h"
#include "layer2_common.h"

#define ROM_FONT_ADDRESS 0x3D00

static void layer2_draw_char(uint8_t x,
                             uint8_t y,
                             char ch,
                             uint8_t color,
                             layer2_screen_t *screen);

static uint8_t *font_address = ROM_FONT_ADDRESS;

void layer2_draw_text(uint8_t row,
                      uint8_t column,
                      const char *text,
                      uint8_t color,
                      layer2_screen_t *screen)
{
    uint8_t x = column << 3;
    uint8_t y = row << 3;
    char *str = (char *) text;

    if ((row > 23) || (column > 31) || (text == NULL))
    {
        return;
    }

    init_switch_screen(screen);

    // If the Spectrum ROM font is used, make sure the Spectrum ROM is switched in.
    if (font_address == (uint8_t *) ROM_FONT_ADDRESS)
    {
        IO_7FFD = IO_7FFD_ROM0;
    }

    if (y < 64)
    {
        switch_top_screen_section(screen);
    }
    else if (y < 128)
    {
        switch_middle_screen_section(screen);
        y -= 64;
    }
    else
    {
        switch_bottom_screen_section(screen);
        y -= 128;
    }

    while (*str != '\0')
    {
        char ch = *str;
        if ((ch < 32) || (ch > 127))
        {
            ch = '?';
        }

        layer2_draw_char(x, y, ch, color, screen);

        if (x == 248)
        {
            break;
        }

        x += 8;
        str++;
    }

    end_switch_screen(screen);
}

void layer2_set_font(const void *new_font_address)
{
    font_address = (new_font_address != NULL) ? (uint8_t *) new_font_address : (uint8_t *) ROM_FONT_ADDRESS;
}

static void layer2_draw_char(uint8_t x,
                             uint8_t y,
                             char ch,
                             uint8_t color,
                             layer2_screen_t *screen)
{
    uint8_t lines = 8;
    uint8_t *source = font_address + ((ch - 32) << 3);
    uint8_t *dest = SCREEN_ADDRESS(screen) + x + (y << 8);

    while (lines--)
    {
        uint8_t i;
        for (i = 0; i < 8; i++)
        {
            uint8_t bit = *source & (0x80 >> i);
            if (bit != 0)
            {
                z80_bpoke(dest + i, color);
            }
        }

        dest += 256;
        source++;
    }
}
