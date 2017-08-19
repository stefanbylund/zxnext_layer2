/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_get_offset_y() in zxnext_layer2.h.
 ******************************************************************************/

#include <stdint.h>

#include "zxnext_layer2.h"
#include "layer2_defs.h"

uint8_t layer2_get_offset_y(void)
{
    IO_REGISTER_NUMBER_PORT = LAYER2_OFFSET_Y_REGISTER;
    return IO_REGISTER_VALUE_PORT;
}
