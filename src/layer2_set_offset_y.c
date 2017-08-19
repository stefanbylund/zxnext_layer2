/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_set_offset_y() in zxnext_layer2.h.
 ******************************************************************************/

#include <stdint.h>

#include "zxnext_layer2.h"
#include "layer2_defs.h"

void layer2_set_offset_y(uint8_t offset_y)
{
    IO_REGISTER_NUMBER_PORT = LAYER2_OFFSET_Y_REGISTER;
    IO_REGISTER_VALUE_PORT = offset_y % 192;
}
