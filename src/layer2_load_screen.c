/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_load_screen() in zxnext_layer2.h.
 ******************************************************************************/

#include <stddef.h>
#include <stdint.h>
#include <errno.h>

// FIXME: Remove when ZEsarUX is updated.
#ifdef __SCCZ80
#define __ESXDOS_DOT_COMMAND
#endif
#include <arch/zx/esxdos.h>

#include "zxnext_layer2.h"
#include "layer2_defs.h"
#include "layer2_common.h"

void layer2_load_screen(const char *filename, off_screen_buffer_t *off_screen_buffer)
{
    uint8_t filehandle;
    uint8_t *dest = SCREEN_ADDRESS(off_screen_buffer);

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

    init_switch_screen(off_screen_buffer);

    switch_top_screen_section(off_screen_buffer);
    esxdos_f_read(filehandle, dest, 16384);
    if (errno)
    {
        goto end;
    }

    switch_middle_screen_section(off_screen_buffer);
    esxdos_f_read(filehandle, dest, 16384);
    if (errno)
    {
        goto end;
    }

    switch_bottom_screen_section(off_screen_buffer);
    esxdos_f_read(filehandle, dest, 16384);

end:
    end_switch_screen(off_screen_buffer);
    esxdos_f_close(filehandle);
}
