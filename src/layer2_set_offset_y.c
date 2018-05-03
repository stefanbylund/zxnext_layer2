/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_set_offset_y() in zxnext_layer2.h.
 ******************************************************************************/

#include <arch/zxn.h>
#include <stdint.h>

#include "zxnext_layer2.h"

void layer2_set_offset_y(uint8_t offset_y)
{
    IO_NEXTREG_REG = REG_LAYER_2_OFFSET_Y;
    IO_NEXTREG_DAT = offset_y % 192;
}
