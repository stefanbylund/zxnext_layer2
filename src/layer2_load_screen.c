/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_load_screen() in zxnext_layer2.h.
 ******************************************************************************/

#include <stddef.h>
#include <stdint.h>
#include <errno.h>
#include <arch/zx/esxdos.h>

#include "zxnext_layer2.h"
#include "layer2_defs.h"
#include "layer2_common.h"

void layer2_load_screen(const char *filename, layer2_screen_t *screen)
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
