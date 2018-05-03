/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_set_global_transparency_color() in zxnext_layer2.h.
 ******************************************************************************/

#include <arch/zxn.h>
#include <stdint.h>

#include "zxnext_layer2.h"

void layer2_set_global_transparency_color(uint8_t color)
{
    IO_NEXTREG_REG = REG_GLOBAL_TRANSPARENCY_COLOR;
    IO_NEXTREG_DAT = color;
}
