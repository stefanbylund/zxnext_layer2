/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Common definitions used by the implementation of zxnext_layer2.h.
 ******************************************************************************/

#ifndef _LAYER2_DEFS_H
#define _LAYER2_DEFS_H

#include <stddef.h>
#include <stdint.h>

#define SCREEN_ADDRESS(screen) ((((screen) != NULL) && ((screen)->screen_type == OFF_SCREEN)) ? (uint8_t *) 0xC000 : (uint8_t *) 0)

#endif
