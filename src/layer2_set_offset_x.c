/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_set_offset_x() in zxnext_layer2.h.
 ******************************************************************************/

#include <stdint.h>

#include "zxnext_layer2.h"
#include "layer2_defs.h"

void layer2_set_offset_x(uint8_t offset_x)
{
    IO_REGISTER_NUMBER_PORT = LAYER2_OFFSET_X_REGISTER;
    IO_REGISTER_VALUE_PORT = offset_x;
}
