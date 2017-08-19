/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_get_offset_x() in zxnext_layer2.h.
 ******************************************************************************/

#include <stdint.h>

#include "zxnext_layer2.h"
#include "layer2_defs.h"

uint8_t layer2_get_offset_x(void)
{
    IO_REGISTER_NUMBER_PORT = LAYER2_OFFSET_X_REGISTER;
    return IO_REGISTER_VALUE_PORT;
}
