/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_load_palette() in zxnext_layer2.h.
 ******************************************************************************/

#include <stddef.h>
#include <stdint.h>
#include <errno.h>
#include <arch/zx/esxdos.h>

#include "zxnext_layer2.h"
#include "layer2_common.h"

void layer2_load_palette(const char *filename)
{
    uint8_t filehandle;

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

end:
    esxdos_f_close(filehandle);
}
