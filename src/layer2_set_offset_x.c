/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_set_offset_x() in zxnext_layer2.h.
 ******************************************************************************/

#include <arch/zxn.h>
#include <stdint.h>

#include "zxnext_layer2.h"

void layer2_set_offset_x(uint8_t offset_x)
{
    IO_NEXTREG_REG = REG_LAYER_2_OFFSET_X;
    IO_NEXTREG_DAT = offset_x;
}
