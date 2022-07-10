/*******************************************************************************
 * Stefan Bylund 2017
 *
 * A layer 2 screen demo program for ZX Spectrum Next.
 *
 * zcc +zxn -subtype=nex -vn -SO3 -startup=30 -clib=sdcc_iy
 *   --max-allocs-per-node200000 -L<zxnext_layer2>/lib/sdcc_iy -lzxnext_layer2
 *   -I<zxnext_layer2>/include zxnext_misc_demo.c -o zxnext_misc_demo -create-app
 ******************************************************************************/

#include <arch/zxn.h>
#include <input.h>
#include <z80.h>
#include <intrinsic.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "zxnext_layer2.h"

#pragma output CRT_ORG_CODE = 0x6164
#pragma output REGISTER_SP = 0xC000
#pragma output CLIB_MALLOC_HEAP_SIZE = 0
#pragma output CLIB_STDIO_HEAP_SIZE = 0
#pragma output CLIB_FOPEN_MAX = -1
#pragma printf = "%c %s"

/*
 * Define IDE_FRIENDLY in your C IDE to disable Z88DK C extensions and avoid
 * parser errors/warnings in the IDE. Do NOT define IDE_FRIENDLY when compiling
 * the code with Z88DK.
 */
#ifdef IDE_FRIENDLY
#define __z88dk_fastcall
#define __preserves_regs(...)
#endif

#define printAt(col, row, str) printf("\x16%c%c%s", (col), (row), (str))

/*******************************************************************************
 * Function Prototypes
 ******************************************************************************/

static void init_hardware(void);

static void init_isr(void);

static void create_start_screen(void);

static void init_tests(void);

static void select_test(void);

static void test_layer2_over_ula(void);

static void test_ula_over_layer2(void);

static void test_main_screen_in_top_16k(void);

static void test_shadow_screen_in_top_16k(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

static uint8_t test_number = 0;

/*******************************************************************************
 * Functions
 ******************************************************************************/

static void init_hardware(void)
{
    // Put Z80 in 28 MHz turbo mode.
    ZXN_NEXTREG(REG_TURBO_MODE, 0x03);

    // Disable RAM memory contention.
    ZXN_NEXTREGA(REG_PERIPHERAL_3, ZXN_READ_REG(REG_PERIPHERAL_3) | RP3_DISABLE_CONTENTION);

    layer2_set_main_screen_ram_bank(8);
    layer2_set_shadow_screen_ram_bank(11);
}

static void init_isr(void)
{
    // Set up IM2 interrupt service routine:
    // Put Z80 in IM2 mode with a 257-byte interrupt vector table located
    // at 0x6000 (before CRT_ORG_CODE) filled with 0x61 bytes. Install an
    // empty interrupt service routine at the interrupt service routine
    // entry at address 0x6161.

    intrinsic_di();
    im2_init((void *) 0x6000);
    memset((void *) 0x6000, 0x61, 257);
    z80_bpoke(0x6161, 0xFB);
    z80_bpoke(0x6162, 0xED);
    z80_bpoke(0x6163, 0x4D);
    intrinsic_ei();
}

static void create_start_screen(void)
{
    zx_border(INK_WHITE);
    zx_cls(INK_BLACK | PAPER_WHITE);

    printAt(5,  7, "Press any key to start");
    printAt(1, 15, "Press any key to switch screen");
}

static void init_tests(void)
{
    zx_border(INK_YELLOW);
    zx_cls(INK_BLACK | PAPER_WHITE);
    layer2_configure(true, false, false, 0);
}

static void select_test(void)
{
    switch (test_number)
    {
        case 0:
            test_layer2_over_ula();
            break;
        case 1:
            test_ula_over_layer2();
            break;
        case 2:
            test_main_screen_in_top_16k();
            break;
        case 3:
            test_shadow_screen_in_top_16k();
            break;
        default:
            break;
    }

    test_number = (test_number + 1) % 4;
}

static void test_layer2_over_ula(void)
{
    // By default, the layer 2 screen is over the ULA screen.
    // Draw a filled rectangle with the transparency colour in the middle
    // of the layer 2 screen so the underlying ULA screen shows through.

    zx_cls(INK_BLACK | PAPER_WHITE);
    printAt(11, 12, "ULA SCREEN");

    layer2_clear_screen(0x37, NULL);
    layer2_draw_text(5, 9, "LAYER 2 SCREEN", 0x88, NULL);
    layer2_fill_rect(64, 80, 128, 40, 0xE3, NULL);

    in_wait_key();
    // Reset to default settings.
    zx_cls(INK_BLACK | PAPER_WHITE);
}

static void test_ula_over_layer2(void)
{
    struct r_Rect8 rect = {8, 16, 10, 5};

    // Set the ULA screen to be over the layer 2 screen.
    // Draw a filled rectangle with the transparency colour in the middle
    // of the ULA screen so the underlying layer 2 screen shows through.

    // FIXME: I can only get ULA transparency to work when
    // the transparency colour is set to black (0x00)...

    zx_cls(INK_BLACK | PAPER_WHITE);
    printAt(11, 5, "ULA SCREEN");
    zx_cls_wc_attr(&rect, PAPER_BLACK);

    layer2_clear_screen(0x37, NULL);
    layer2_draw_text(12, 9, "LAYER 2 SCREEN", 0x88, NULL);

    layer2_set_layer_priorities(LAYER_PRIORITIES_S_U_L);
    layer2_set_global_transparency_color(0x00);

    in_wait_key();
    // Reset to default settings.
    zx_cls(INK_BLACK | PAPER_WHITE);
    layer2_set_layer_priorities(LAYER_PRIORITIES_S_L_U);
    layer2_set_global_transparency_color(0xE3);
}

/*
 * This function demonstrates that it is possible to page the main layer 2
 * screen to the top 16K RAM instead of the bottom 16K and still have any
 * drawing results directly displayed.
 */
static void test_main_screen_in_top_16k(void)
{
    layer2_screen_t main_screen_buffer = {OFF_SCREEN, 8, 9, 10};

    layer2_fill_rect(0, 0,   256, 64, 0xFE, &main_screen_buffer);
    layer2_fill_rect(0, 64,  256, 64, 0x7E, &main_screen_buffer);
    layer2_fill_rect(0, 128, 256, 64, 0x9F, &main_screen_buffer);

    layer2_draw_rect(24, 32, 208, 128, 0x6F, &main_screen_buffer);
    layer2_draw_text(12, 5, "Main screen in top 16K", 0x00, &main_screen_buffer);
}

/*
 * This function demonstrates that it is possible to page the shadow layer 2
 * screen (or any layer 2 off-screen buffer stored in consecutive RAM banks)
 * to the top 16K RAM instead of the bottom 16K. We have to flip the main and
 * shadow screens to display the results of the drawing.
 */
static void test_shadow_screen_in_top_16k(void)
{
    layer2_screen_t shadow_screen_buffer = {OFF_SCREEN, 11, 12, 13};

    layer2_fill_rect(0, 0,   256, 64, 0xFE, &shadow_screen_buffer);
    layer2_fill_rect(0, 64,  256, 64, 0x7E, &shadow_screen_buffer);
    layer2_fill_rect(0, 128, 256, 64, 0x9F, &shadow_screen_buffer);

    layer2_draw_rect(24, 32, 208, 128, 0x6F, &shadow_screen_buffer);
    layer2_draw_text(12, 4, "Shadow screen in top 16K", 0x00, &shadow_screen_buffer);

    layer2_flip_main_shadow_screen();
}

int main(void)
{
    init_hardware();
    init_isr();

    create_start_screen();
    in_wait_key();
    init_tests();

    while (true)
    {
        if (in_inkey())
        {
            in_wait_nokey();
            select_test();
        }
    }
}
