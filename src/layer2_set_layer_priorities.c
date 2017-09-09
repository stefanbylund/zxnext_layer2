/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_set_layer_priorities() in zxnext_layer2.h.
 ******************************************************************************/

#include <stdint.h>

#include "zxnext_layer2.h"
#include "layer2_defs.h"

// FIXME: We should first read SPRITE_LAYERS_SYSTEM_REGISTER and then mask in
// the specified layer_priorities value to preserve the other bits. However,
// the CSpect emulator doesn't seem to support reading this register.
void layer2_set_layer_priorities(uint8_t layer_priorities)
{
    IO_REGISTER_NUMBER_PORT = SPRITE_LAYERS_SYSTEM_REGISTER;
    IO_REGISTER_VALUE_PORT = (layer_priorities & LAYER_PRIORITIES_MASK) << LAYER_PRIORITIES_SHIFT;
}
