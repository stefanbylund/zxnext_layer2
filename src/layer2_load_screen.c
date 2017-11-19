/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_load_screen() in zxnext_layer2.h.
 ******************************************************************************/

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <arch/zx/esxdos.h>

#include "zxnext_layer2.h"
#include "layer2_defs.h"
#include "layer2_common.h"

void layer2_load_screen(const char *filename, layer2_screen_t *screen, bool has_palette)
{
    uint8_t filehandle;
    uint8_t *dest = SCREEN_ADDRESS(screen);

    if (filename == NULL)
    {
        return;
    }

    errno = 0;
    filehandle = esxdos_f_open(filename, ESXDOS_MODE_R | ESXDOS_MODE_OE);
    if (errno)
    {
        return;
    }

    init_switch_screen(screen);

    if (has_palette)
    {
        esxdos_f_read(filehandle, buf_256, 256);
        if (errno)
        {
            goto end;
        }
        layer2_set_palette((uint16_t *) buf_256, 128, 0);

        esxdos_f_read(filehandle, buf_256, 256);
        if (errno)
        {
            goto end;
        }
        layer2_set_palette((uint16_t *) buf_256, 128, 128);
    }

    switch_top_screen_section(screen);
    esxdos_f_read(filehandle, dest, 16384);
    if (errno)
    {
        goto end;
    }

    switch_middle_screen_section(screen);
    esxdos_f_read(filehandle, dest, 16384);
    if (errno)
    {
        goto end;
    }

    switch_bottom_screen_section(screen);
    esxdos_f_read(filehandle, dest, 16384);

end:
    end_switch_screen(screen);
    esxdos_f_close(filehandle);
}
