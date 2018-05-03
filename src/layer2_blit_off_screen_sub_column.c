/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_blit_off_screen_sub_column() in zxnext_layer2.h.
 ******************************************************************************/

#include <arch/zxn.h>
#include <z80.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "zxnext_layer2.h"
#include "layer2_common.h"

static void layer2_blit_sub_column(uint8_t dest_x,
                                   uint8_t dest_y,
                                   uint8_t source_x,
                                   uint8_t source_y,
                                   uint8_t height);

static void layer2_blit_sub_column_section(uint8_t dest_x,
                                           uint8_t dest_y,
                                           uint8_t source_x,
                                           uint8_t source_y,
                                           uint8_t height);

void layer2_blit_off_screen_sub_column(uint8_t dest_x,
                                       uint8_t dest_y,
                                       layer2_screen_t *source,
                                       uint8_t source_x,
                                       uint8_t source_y,
                                       uint8_t height)
{
    if ((dest_y > 191) || (source_y > 191) || (height == 0) ||
        (source == NULL) || (source->screen_type != OFF_SCREEN))
    {
        return;
    }

    if (source_y + height - 1 > 191)
    {
        height = 191 - source_y + 1;
    }

    source->tmp = z80_bpeek(__SYSVAR_BANKM);

    if (source_y < 64)
    {
        // top || top + middle || top + middle + bottom
        if (source_y + height - 1 < 64)
        {
            // top
            switch_ram_bank(source->top_bank);
            layer2_blit_sub_column(dest_x,
                                   dest_y,
                                   source_x,
                                   source_y,
                                   height);
        }
        else if (source_y + height - 1 < 128)
        {
            // top
            switch_ram_bank(source->top_bank);
            layer2_blit_sub_column(dest_x,
                                   dest_y,
                                   source_x,
                                   source_y,
                                   64 - source_y);

            // middle
            switch_ram_bank(source->middle_bank);
            layer2_blit_sub_column(dest_x,
                                   dest_y + (64 - source_y),
                                   source_x,
                                   0,
                                   height - (64 - source_y));
        }
        else
        {
            // top
            switch_ram_bank(source->top_bank);
            layer2_blit_sub_column(dest_x,
                                   dest_y,
                                   source_x,
                                   source_y,
                                   64 - source_y);

            // middle
            switch_ram_bank(source->middle_bank);
            layer2_blit_sub_column(dest_x,
                                   dest_y + (64 - source_y),
                                   source_x,
                                   0,
                                   64);

            // bottom
            switch_ram_bank(source->bottom_bank);
            layer2_blit_sub_column(dest_x,
                                   dest_y + (128 - source_y),
                                   source_x,
                                   0,
                                   height - (128 - source_y));
        }
    }
    else if (source_y < 128)
    {
        // middle || middle + bottom
        if (source_y + height - 1 < 128)
        {
            // middle
            switch_ram_bank(source->middle_bank);
            layer2_blit_sub_column(dest_x,
                                   dest_y,
                                   source_x,
                                   source_y - 64,
                                   height);
        }
        else
        {
            // middle
            switch_ram_bank(source->middle_bank);
            layer2_blit_sub_column(dest_x,
                                   dest_y,
                                   source_x,
                                   source_y - 64,
                                   128 - source_y);

            // bottom
            switch_ram_bank(source->bottom_bank);
            layer2_blit_sub_column(dest_x,
                                   dest_y + (128 - source_y),
                                   source_x,
                                   0,
                                   height - (128 - source_y));
        }
    }
    else
    {
        // bottom
        switch_ram_bank(source->bottom_bank);
        layer2_blit_sub_column(dest_x,
                               dest_y,
                               source_x,
                               source_y - 128,
                               height);
    }

    switch_ram_bank(source->tmp);
}

static void layer2_blit_sub_column(uint8_t dest_x,
                                   uint8_t dest_y,
                                   uint8_t source_x,
                                   uint8_t source_y,
                                   uint8_t height)
{
    // Destination is the main screen.
    // Source is one section of an off-screen buffer, i.e. height <= 64.

    if (dest_y + height - 1 > 191)
    {
        height = 191 - dest_y + 1;
    }

    if (dest_y < 64)
    {
        // top || top + middle
        if (dest_y + height - 1 < 64)
        {
            // top
            layer2_configure(true, true, false, LAYER2_SCREEN_TOP);
            layer2_blit_sub_column_section(dest_x,
                                           dest_y,
                                           source_x,
                                           source_y,
                                           height);
        }
        else
        {
            // top
            layer2_configure(true, true, false, LAYER2_SCREEN_TOP);
            layer2_blit_sub_column_section(dest_x,
                                           dest_y,
                                           source_x,
                                           source_y,
                                           64 - dest_y);

            // middle
            layer2_configure(true, true, false, LAYER2_SCREEN_MIDDLE);
            layer2_blit_sub_column_section(dest_x,
                                           0,
                                           source_x,
                                           source_y + (64 - dest_y),
                                           height - (64 - dest_y));
        }
    }
    else if (dest_y < 128)
    {
        // middle || middle + bottom
        if (dest_y + height - 1 < 128)
        {
            // middle
            layer2_configure(true, true, false, LAYER2_SCREEN_MIDDLE);
            layer2_blit_sub_column_section(dest_x,
                                           dest_y - 64,
                                           source_x,
                                           source_y,
                                           height);
        }
        else
        {
            // middle
            layer2_configure(true, true, false, LAYER2_SCREEN_MIDDLE);
            layer2_blit_sub_column_section(dest_x,
                                           dest_y - 64,
                                           source_x,
                                           source_y,
                                           128 - dest_y);

            // bottom
            layer2_configure(true, true, false, LAYER2_SCREEN_BOTTOM);
            layer2_blit_sub_column_section(dest_x,
                                           0,
                                           source_x,
                                           source_y + (128 - dest_y),
                                           height - (128 - dest_y));
        }
    }
    else
    {
        // bottom
        layer2_configure(true, true, false, LAYER2_SCREEN_BOTTOM);
        layer2_blit_sub_column_section(dest_x,
                                       dest_y - 128,
                                       source_x,
                                       source_y,
                                       height);
    }

    layer2_configure(true, false, false, 0);
}

static void layer2_blit_sub_column_section(uint8_t dest_x,
                                           uint8_t dest_y,
                                           uint8_t source_x,
                                           uint8_t source_y,
                                           uint8_t height)
{
    // Destination is one section of the main screen.
    // Source is one section of an off-screen buffer.

    uint8_t *dest = (uint8_t *) (dest_x + (dest_y << 8));
    uint8_t *source = (uint8_t *) (0xC000 + source_x + (source_y << 8));

    while (height--)
    {
        z80_bpoke(dest, *source);
        dest += 256;
        source += 256;
    }
}
