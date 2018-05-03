/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_load_screen() in zxnext_layer2.h.
 ******************************************************************************/

#include <arch/zxn.h>
#include <arch/zxn/esxdos.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include "zxnext_layer2.h"
#include "layer2_common.h"

static void get_screen_pages(layer2_screen_t *screen, uint8_t *pages);

static uint16_t mmu_address(uint8_t mmu_slot);

static uint8_t mmu_default_page(uint8_t mmu_slot);

void layer2_load_screen(const char *filename, layer2_screen_t *screen, uint8_t mmu_slot, bool has_palette)
{
    uint8_t filehandle;
    uint8_t old_page;
    uint8_t screen_pages[6];
    void *screen_address;
    uint8_t i;

    if ((filename == NULL) || (mmu_slot < 2) || (mmu_slot > 7))
    {
        return;
    }

    // Disable layer 2 screen writing at bottom 16 KB prior to ESXDOS usage.
    layer2_configure(true, false, false, 0);

    errno = 0;
    filehandle = esxdos_f_open(filename, ESXDOS_MODE_R | ESXDOS_MODE_OE);
    if (errno)
    {
        return;
    }

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

    // Load layer 2 screen in 8 KB chunks using the given MMU slot.

    // FIXME: Read and save current page when supported by CSpect.
    old_page = mmu_default_page(mmu_slot);
    get_screen_pages(screen, screen_pages);
    screen_address = (void *) mmu_address(mmu_slot);

    for (i = 0; i < 6; i++)
    {
        ZXN_WRITE_REG(REG_MMU0 + mmu_slot, screen_pages[i]);
        esxdos_f_read(filehandle, screen_address, 8192);
        if (errno)
        {
            break;
        }
    }

    ZXN_WRITE_REG(REG_MMU0 + mmu_slot, old_page);

end:
    esxdos_f_close(filehandle);
}

// TODO: Remove if layer2_screen_t is changed and off-screen banks are consecutive.
static void get_screen_pages(layer2_screen_t *screen, uint8_t *pages)
{
    if (!IS_OFF_SCREEN(screen))
    {
        uint8_t screen_start_page;
        uint8_t i;

        screen_start_page = IS_MAIN_SCREEN(screen) ?
            layer2_get_main_screen_ram_bank() : layer2_get_shadow_screen_ram_bank();
        screen_start_page = screen_start_page << 1;

        for (i = 0; i < 6; i++)
        {
            pages[i] = screen_start_page + i;
        }
    }
    else
    {
        pages[0] = screen->top_bank << 1;
        pages[1] = pages[0] + 1;
        pages[2] = screen->middle_bank << 1;
        pages[3] = pages[2] + 1;
        pages[4] = screen->bottom_bank << 1;
        pages[5] = pages[4] + 1;
    }
}

// FIXME: Remove when zxn_addr_from_mmu() works in z88dk.
static uint16_t mmu_address(uint8_t mmu_slot)
{
    switch (mmu_slot)
    {
        case 0:
            return 0;
        case 1:
            return 0x2000;
        case 2:
            return 0x4000;
        case 3:
            return 0x6000;
        case 4:
            return 0x8000;
        case 5:
            return 0xA000;
        case 6:
            return 0xC000;
        case 7:
            return 0xE000;
        default:
            return 0;
    }
}

// FIXME: Remove when CSpect supports reading MMU slot registers.
static uint8_t mmu_default_page(uint8_t mmu_slot)
{
    switch (mmu_slot)
    {
        case 0:
            return 255;
        case 1:
            return 255;
        case 2:
            return 10;
        case 3:
            return 11;
        case 4:
            return 4;
        case 5:
            return 5;
        case 6:
            return 0;
        case 7:
            return 1;
        default:
            return 0;
    }
}
