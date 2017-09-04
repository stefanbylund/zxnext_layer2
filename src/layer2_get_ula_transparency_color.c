/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_get_global_transparency_color() in zxnext_layer2.h.
 ******************************************************************************/

#include <stdint.h>

#include "zxnext_layer2.h"
#include "layer2_defs.h"

uint8_t layer2_get_global_transparency_color(void)
{
    IO_REGISTER_NUMBER_PORT = GLOBAL_TRANSPARENCY_COLOR_REGISTER;
    return IO_REGISTER_VALUE_PORT;
}
