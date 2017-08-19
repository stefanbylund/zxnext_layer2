/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_draw_line() in zxnext_layer2.h.
 ******************************************************************************/

#include <stdint.h>

#include "zxnext_layer2.h"
#include "layer2_common.h"
#include "layer2_draw_pixel_fast.h"

// http://www.edepot.com/linebresenham.html
void layer2_draw_line(uint8_t x1,
                      uint8_t y1,
                      uint8_t x2,
                      uint8_t y2,
                      uint8_t color,
                      off_screen_buffer_t *off_screen_buffer)
{
    uint16_t dx;
    uint16_t dy;
    int8_t incx;
    int8_t incy;
    int16_t balance;

    if ((y1 > 191) || (y2 > 191))
    {
        return;
    }

    if (x2 >= x1)
    {
        dx = x2 - x1;
        incx = 1;
    }
    else
    {
        dx = x1 - x2;
        incx = -1;
    }

    if (y2 >= y1)
    {
        dy = y2 - y1;
        incy = 1;
    }
    else
    {
        dy = y1 - y2;
        incy = -1;
    }

    init_switch_screen(off_screen_buffer);

    if (dx >= dy)
    {
        dy <<= 1;
        balance = dy - dx;
        dx <<= 1;

        while (x1 != x2)
        {
            layer2_draw_pixel_fast(x1, y1, color, off_screen_buffer);
            if (balance >= 0)
            {
                y1 += incy;
                balance -= dx;
            }
            balance += dy;
            x1 += incx;
        }

        layer2_draw_pixel_fast(x1, y1, color, off_screen_buffer);
    }
    else
    {
        dx <<= 1;
        balance = dx - dy;
        dy <<= 1;

        while (y1 != y2)
        {
            layer2_draw_pixel_fast(x1, y1, color, off_screen_buffer);
            if (balance >= 0)
            {
                x1 += incx;
                balance -= dy;
            }
            balance += dx;
            y1 += incy;
        }

        layer2_draw_pixel_fast(x1, y1, color, off_screen_buffer);
    }

    end_switch_screen(off_screen_buffer);
}
