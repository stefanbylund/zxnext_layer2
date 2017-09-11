/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Definition of helper function layer2_draw_pixel_fast().
 *
 * The caller of this function is required to validate its arguments and make
 * sure it is called within init_switch_screen() and end_switch_screen().
 ******************************************************************************/

#ifndef _LAYER2_DRAW_PIXEL_FAST_H
#define _LAYER2_DRAW_PIXEL_FAST_H

#include <stdint.h>
#include "zxnext_layer2.h"

void layer2_draw_pixel_fast(uint8_t x,
                            uint8_t y,
                            uint8_t color,
                            layer2_screen_t *screen);

#endif
