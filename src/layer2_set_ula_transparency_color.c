/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_set_ula_transparency_color() in zxnext_layer2.h.
 ******************************************************************************/

#include <stdint.h>

#include "zxnext_layer2.h"
#include "layer2_defs.h"

void layer2_set_ula_transparency_color(uint8_t color)
{
    IO_REGISTER_NUMBER_PORT = LAYER2_TRANSPARENCY_COLOR_REGISTER;
    IO_REGISTER_VALUE_PORT = color & LAYER2_ULA_TRANSPARENCY_COLOR_MASK;
}
