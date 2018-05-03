/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_configure() in zxnext_layer2.h.
 ******************************************************************************/

#include <arch/zxn.h>
#include <stdint.h>
#include <stdbool.h>

#include "zxnext_layer2.h"

#define LAYER2_VISIBLE_MASK 0x02
#define LAYER2_SHADOW_SCREEN_MASK 0x08
#define LAYER2_WRITE_ENABLE_MASK 0x01
#define LAYER2_SCREEN_SECTION_MASK 0x03
#define LAYER2_SCREEN_SECTION_SHIFT 6

/*
 * (R/W) Layer 2 screen configuration port (0x123B):
 * bits 6-7 = Section (0 = top, 1 = middle, 2 = bottom) of the specified layer 2
 * screen (main or shadow) to page in for writing.
 * bits 4-5 = Reserved, must be 0.
 * bit 3 = If 0, page in main layer 2 screen as specified by LAYER2_RAM_PAGE_REGISTER.
 * If 1, page in shadow layer 2 screen as specified by LAYER2_SHADOW_RAM_PAGE_REGISTER.
 * bit 2 = Reserved, must be 0.
 * bit 1 = Layer 2 screen visible on the display.
 * bit 0 = Enable the paged-in layer 2 screen (main or shadow) for writing.
 */

// FIXME: If the LAYER2_PORT is also readable, we can split this function into
// two functions; one for enabling the layer 2 screen and one for paging it in.
// Then we should also preserve the other bits when setting the layer 2 screen
// section for writing.

void layer2_configure(bool layer2_visible,
                      bool layer2_write_enabled,
                      bool layer2_shadow_screen,
                      uint8_t layer2_screen_section)
{
    uint8_t value = (layer2_screen_section & LAYER2_SCREEN_SECTION_MASK) << LAYER2_SCREEN_SECTION_SHIFT;

    if (layer2_visible)
    {
        value = value | LAYER2_VISIBLE_MASK;
    }

    if (layer2_write_enabled)
    {
        value = value | LAYER2_WRITE_ENABLE_MASK;
    }

    if (layer2_shadow_screen)
    {
        value = value | LAYER2_SHADOW_SCREEN_MASK;
    }

    IO_LAYER_2_CONFIG = value;
}
