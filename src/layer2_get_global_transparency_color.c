/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_get_global_transparency_color() in zxnext_layer2.h.
 ******************************************************************************/

#include <arch/zxn.h>
#include <stdint.h>

#include "zxnext_layer2.h"

uint8_t layer2_get_global_transparency_color(void)
{
    IO_NEXTREG_REG = REG_GLOBAL_TRANSPARENCY_COLOR;
    return IO_NEXTREG_DAT;
}
