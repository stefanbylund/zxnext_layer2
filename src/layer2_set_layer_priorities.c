/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_set_layer_priorities() in zxnext_layer2.h.
 ******************************************************************************/

#include <stdint.h>

#include "zxnext_layer2.h"
#include "layer2_defs.h"

void layer2_set_layer_priorities(uint8_t layer_priorities)
{
    IO_REGISTER_NUMBER_PORT = SPRITE_LAYERS_SYSTEM_REGISTER;
    IO_REGISTER_VALUE_PORT = (IO_REGISTER_VALUE_PORT & 0xE3) |
        ((layer_priorities & LAYER_PRIORITIES_MASK) << LAYER_PRIORITIES_SHIFT);
}
