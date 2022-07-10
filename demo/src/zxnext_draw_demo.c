/*******************************************************************************
 * Stefan Bylund 2017
 *
 * A layer 2 drawing demo program for ZX Spectrum Next.
 *
 * zcc +zxn -subtype=nex -vn -SO3 -startup=30 -clib=sdcc_iy
 *   --max-allocs-per-node200000 -L<zxnext_layer2>/lib/sdcc_iy -lzxnext_layer2
 *   -I<zxnext_layer2>/include zxnext_draw_demo.c -o zxnext_draw_demo -create-app
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

static void test_clear_screen(layer2_screen_t *screen);

static void test_load_screen(layer2_screen_t *screen);

static void test_load_screen_with_palette(layer2_screen_t *screen);

static void test_draw_pixel(layer2_screen_t *screen);

static void test_draw_line(layer2_screen_t *screen);

static void test_draw_rect(layer2_screen_t *screen);

static void test_fill_rect(layer2_screen_t *screen);

static void test_draw_text(layer2_screen_t *screen);

static void test_blit(layer2_screen_t *screen);

static void test_blit_transparent(layer2_screen_t *screen);

/*******************************************************************************
 * Variables
 ******************************************************************************/

static uint8_t test_number = 0;

static layer2_screen_t shadow_screen = {SHADOW_SCREEN};

static layer2_screen_t off_screen = {OFF_SCREEN, 0, 1, 3};

static uint8_t tall_sprite[192];

static const uint8_t sprite[] =
{
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3,
    0x04, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x04, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3,
    0x04, 0xFF, 0xFB, 0xFB, 0xFB, 0xFF, 0x04, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3,
    0x04, 0xFF, 0xFB, 0xF5, 0xF5, 0xFB, 0xFF, 0x04, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3,
    0x04, 0xFF, 0xFB, 0xF5, 0xA8, 0xA8, 0xFB, 0xFF, 0x04, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3,
    0x04, 0xFF, 0xFF, 0xFB, 0xA8, 0x44, 0xA8, 0xFB, 0xFF, 0x04, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3,
    0x04, 0x04, 0x04, 0xFF, 0xFB, 0xA8, 0x44, 0xA8, 0xFB, 0xFF, 0x04, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3,
    0xE3, 0xE3, 0xE3, 0x04, 0xFF, 0xFB, 0xA8, 0x44, 0x44, 0xFB, 0xFF, 0x04, 0xE3, 0x04, 0xE3, 0xE3,
    0xE3, 0xE3, 0xE3, 0xE3, 0x04, 0xFF, 0xFB, 0x44, 0x44, 0x44, 0xFB, 0xFF, 0x04, 0x4D, 0x04, 0xE3,
    0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0x04, 0xFF, 0xFB, 0x44, 0x44, 0x44, 0x44, 0xFA, 0x4D, 0x04, 0xE3,
    0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0x04, 0xFF, 0xFB, 0x44, 0xFF, 0xF5, 0x44, 0x04, 0xE3, 0xE3,
    0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0x04, 0xFF, 0x44, 0xF5, 0xA8, 0x04, 0xE3, 0xE3, 0xE3,
    0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0x04, 0xFA, 0x44, 0x04, 0xA8, 0x04, 0xE3, 0xE3,
    0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0x04, 0x4D, 0x4D, 0x04, 0xE3, 0x04, 0xF5, 0x04, 0xE3,
    0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0x04, 0x04, 0xE3, 0xE3, 0xE3, 0x04, 0xFA, 0x04,
    0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0x04, 0x04
};

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

    memset(tall_sprite, 0x27, 192);
    layer2_configure(true, false, false, 0);
}

static void select_test(void)
{
    switch (test_number)
    {
        case 0:
            test_clear_screen(NULL);
            break;
        case 1:
            test_load_screen(NULL);
            break;
        case 2:
            test_load_screen_with_palette(NULL);
            break;
        case 3:
            test_draw_pixel(NULL);
            break;
        case 4:
            test_draw_line(NULL);
            break;
        case 5:
            test_draw_rect(NULL);
            break;
        case 6:
            test_fill_rect(NULL);
            break;
        case 7:
            test_draw_text(NULL);
            break;
        case 8:
            test_blit(NULL);
            break;
        case 9:
            test_blit_transparent(NULL);
            break;
        case 10:
            test_clear_screen(&shadow_screen);
            break;
        case 11:
            test_load_screen(&shadow_screen);
            break;
        case 12:
            test_load_screen_with_palette(&shadow_screen);
            break;
        case 13:
            test_draw_pixel(&shadow_screen);
            break;
        case 14:
            test_draw_line(&shadow_screen);
            break;
        case 15:
            test_draw_rect(&shadow_screen);
            break;
        case 16:
            test_fill_rect(&shadow_screen);
            break;
        case 17:
            test_draw_text(&shadow_screen);
            break;
        case 18:
            test_blit(&shadow_screen);
            break;
        case 19:
            test_blit_transparent(&shadow_screen);
            break;
        case 20:
            test_clear_screen(&off_screen);
            break;
        case 21:
            test_load_screen(&off_screen);
            break;
        case 22:
            test_load_screen_with_palette(&off_screen);
            break;
        case 23:
            test_draw_pixel(&off_screen);
            break;
        case 24:
            test_draw_line(&off_screen);
            break;
        case 25:
            test_draw_rect(&off_screen);
            break;
        case 26:
            test_fill_rect(&off_screen);
            break;
        case 27:
            test_draw_text(&off_screen);
            break;
        case 28:
            test_blit(&off_screen);
            break;
        case 29:
            test_blit_transparent(&off_screen);
            break;
        default:
            break;
    }

    test_number = (test_number + 1) % 30;
}

static void test_clear_screen(layer2_screen_t *screen)
{
    layer2_clear_screen(0xDB, screen);

    if (IS_SHADOW_SCREEN(screen))
    {
        layer2_flip_main_shadow_screen();
    }
    else if (IS_OFF_SCREEN(screen))
    {
        layer2_copy_off_screen(screen);
    }
}

static void test_load_screen(layer2_screen_t *screen)
{
    layer2_load_screen("screen1.nxi", screen, 7, false);

    if (IS_SHADOW_SCREEN(screen))
    {
        layer2_flip_main_shadow_screen();
    }
    else if (IS_OFF_SCREEN(screen))
    {
        layer2_copy_off_screen(screen);
    }
}

static void test_load_screen_with_palette(layer2_screen_t *screen)
{
    // Load screen with palette using second layer 2 palette.
    layer2_set_display_palette(false);
    layer2_set_rw_palette(false);
    layer2_load_screen("img_pal.nxi", screen, 7, true);

    if (IS_SHADOW_SCREEN(screen))
    {
        layer2_flip_main_shadow_screen();
    }
    else if (IS_OFF_SCREEN(screen))
    {
        layer2_copy_off_screen(screen);
    }

    in_wait_key();
    // Reset to first layer 2 palette.
    layer2_set_display_palette(true);
    layer2_set_rw_palette(true);
}

static void test_draw_pixel(layer2_screen_t *screen)
{
    layer2_fill_rect(0, 0,   256, 64, 0xFE, screen);
    layer2_fill_rect(0, 64,  256, 64, 0x7E, screen);
    layer2_fill_rect(0, 128, 256, 64, 0x9F, screen);

    layer2_draw_pixel(0,   0,  0x03, screen);
    layer2_draw_pixel(127, 0,  0x03, screen);
    layer2_draw_pixel(255, 0,  0x03, screen);
    layer2_draw_pixel(0,   31, 0x03, screen);
    layer2_draw_pixel(127, 31, 0x03, screen);
    layer2_draw_pixel(255, 31, 0x03, screen);
    layer2_draw_pixel(0,   63, 0x03, screen);
    layer2_draw_pixel(127, 63, 0x03, screen);
    layer2_draw_pixel(255, 63, 0x03, screen);

    layer2_draw_pixel(0,   64,  0xE0, screen);
    layer2_draw_pixel(127, 64,  0xE0, screen);
    layer2_draw_pixel(255, 64,  0xE0, screen);
    layer2_draw_pixel(0,   95,  0xE0, screen);
    layer2_draw_pixel(127, 95,  0xE0, screen);
    layer2_draw_pixel(255, 95,  0xE0, screen);
    layer2_draw_pixel(0,   127, 0xE0, screen);
    layer2_draw_pixel(127, 127, 0xE0, screen);
    layer2_draw_pixel(255, 127, 0xE0, screen);

    layer2_draw_pixel(0,   128, 0x83, screen);
    layer2_draw_pixel(127, 128, 0x83, screen);
    layer2_draw_pixel(255, 128, 0x83, screen);
    layer2_draw_pixel(0,   159, 0x83, screen);
    layer2_draw_pixel(127, 159, 0x83, screen);
    layer2_draw_pixel(255, 159, 0x83, screen);
    layer2_draw_pixel(0,   191, 0x83, screen);
    layer2_draw_pixel(127, 191, 0x83, screen);
    layer2_draw_pixel(255, 191, 0x83, screen);

    // Outside screen.
    layer2_draw_pixel(255, 255, 0x83, screen);

    if (IS_SHADOW_SCREEN(screen))
    {
        layer2_flip_main_shadow_screen();
    }
    else if (IS_OFF_SCREEN(screen))
    {
        layer2_copy_off_screen(screen);
    }
}

static void test_draw_line(layer2_screen_t *screen)
{
    layer2_fill_rect(0, 0,   256, 64, 0xFE, screen);
    layer2_fill_rect(0, 64,  256, 64, 0x7E, screen);
    layer2_fill_rect(0, 128, 256, 64, 0x9F, screen);

    layer2_draw_line(0,   0,   255, 0,   0x6F, screen);
    layer2_draw_line(0,   191, 255, 191, 0x6F, screen);
    layer2_draw_line(0,   0,   0,   191, 0x6F, screen);
    layer2_draw_line(255, 0,   255, 191, 0x6F, screen);

    layer2_draw_line(0,   0, 255, 191, 0xE0, screen);
    layer2_draw_line(255, 0, 0,   191, 0xE0, screen);

    // Outside screen.
    layer2_draw_line(127, 0, 255, 255, 0xE0, screen);

    if (IS_SHADOW_SCREEN(screen))
    {
        layer2_flip_main_shadow_screen();
    }
    else if (IS_OFF_SCREEN(screen))
    {
        layer2_copy_off_screen(screen);
    }
}

static void test_draw_rect(layer2_screen_t *screen)
{
    layer2_fill_rect(0, 0,   256, 64, 0xFE, screen);
    layer2_fill_rect(0, 64,  256, 64, 0x7E, screen);
    layer2_fill_rect(0, 128, 256, 64, 0x9F, screen);

    layer2_draw_rect(0, 0, 256, 192, 0x6F, screen);

    layer2_draw_rect(118, 22,  20, 20, 0xE0, screen);
    layer2_draw_rect(118, 54,  20, 20, 0xE0, screen);
    layer2_draw_rect(118, 86,  20, 20, 0xE0, screen);
    layer2_draw_rect(118, 118, 20, 20, 0xE0, screen);
    layer2_draw_rect(118, 150, 20, 20, 0xE0, screen);
    layer2_draw_rect(118, 182, 20, 20, 0xE0, screen);

    // Clipped rectangles.
    layer2_draw_rect(246, 54,  20, 20, 0xEF, screen);
    layer2_draw_rect(246, 118, 20, 20, 0xEF, screen);
    layer2_draw_rect(246, 182, 20, 20, 0xEF, screen);

    // Outside screen.
    layer2_draw_rect(246, 246, 20, 20, 0xEF, screen);

    if (IS_SHADOW_SCREEN(screen))
    {
        layer2_flip_main_shadow_screen();
    }
    else if (IS_OFF_SCREEN(screen))
    {
        layer2_copy_off_screen(screen);
    }
}

static void test_fill_rect(layer2_screen_t *screen)
{
    layer2_fill_rect(0, 0,   256, 64, 0xFE, screen);
    layer2_fill_rect(0, 64,  256, 64, 0x7E, screen);
    layer2_fill_rect(0, 128, 256, 64, 0x9F, screen);

    layer2_fill_rect(118, 22,  20, 20, 0xE0, screen);
    layer2_fill_rect(118, 54,  20, 20, 0xE0, screen);
    layer2_fill_rect(118, 86,  20, 20, 0xE0, screen);
    layer2_fill_rect(118, 118, 20, 20, 0xE0, screen);
    layer2_fill_rect(118, 150, 20, 20, 0xE0, screen);
    layer2_fill_rect(118, 182, 20, 20, 0xE0, screen);

    // Clipped rectangles.
    layer2_fill_rect(246, 54,  20, 20, 0xEF, screen);
    layer2_fill_rect(246, 118, 20, 20, 0xEF, screen);
    layer2_fill_rect(246, 182, 20, 20, 0xEF, screen);

    // Outside screen.
    layer2_fill_rect(246, 246, 20, 20, 0xEF, screen);

    if (IS_SHADOW_SCREEN(screen))
    {
        layer2_flip_main_shadow_screen();
    }
    else if (IS_OFF_SCREEN(screen))
    {
        layer2_copy_off_screen(screen);
    }
}

static void test_draw_text(layer2_screen_t *screen)
{
    layer2_fill_rect(0, 0,   256, 64, 0xFE, screen);
    layer2_fill_rect(0, 64,  256, 64, 0x7E, screen);
    layer2_fill_rect(0, 128, 256, 64, 0x9F, screen);

    layer2_draw_text(3,  12, "Hello", 0x00, screen);
    layer2_set_font(NULL);
    layer2_draw_text(11, 12, "Hello", 0x00, screen);
    layer2_set_font((void *) 0x3D00);
    layer2_draw_text(19, 12, "Hello", 0x00, screen);

    // Clipped text.
    layer2_draw_text(3,  29, "Hello", 0xEF, screen);
    layer2_draw_text(11, 29, "Hello", 0xEF, screen);
    layer2_draw_text(19, 29, "Hello", 0xEF, screen);

    // Outside screen.
    layer2_draw_text(24, 32, "Hello", 0xEF, screen);

    if (IS_SHADOW_SCREEN(screen))
    {
        layer2_flip_main_shadow_screen();
    }
    else if (IS_OFF_SCREEN(screen))
    {
        layer2_copy_off_screen(screen);
    }
}

static void test_blit(layer2_screen_t *screen)
{
    layer2_fill_rect(0, 0,   256, 64, 0xFE, screen);
    layer2_fill_rect(0, 64,  256, 64, 0x7E, screen);
    layer2_fill_rect(0, 128, 256, 64, 0x9F, screen);

    layer2_blit(120, 24,  sprite, 16, 16, screen); // top
    layer2_blit(120, 56,  sprite, 16, 16, screen); // top + middle
    layer2_blit(120, 88,  sprite, 16, 16, screen); // middle
    layer2_blit(120, 120, sprite, 16, 16, screen); // middle + bottom
    layer2_blit(120, 152, sprite, 16, 16, screen); // bottom
    layer2_blit(120, 184, sprite, 16, 16, screen); // bottom clipped

    layer2_blit(64, 48, tall_sprite, 2, 96, screen); // top + middle + bottom

    // Clipped blits.
    layer2_blit(248, 56,  sprite, 16, 16, screen);
    layer2_blit(248, 120, sprite, 16, 16, screen);
    layer2_blit(248, 184, sprite, 16, 16, screen);

    // Outside screen.
    layer2_blit(248, 248, sprite, 16, 16, screen);

    if (IS_SHADOW_SCREEN(screen))
    {
        layer2_flip_main_shadow_screen();
    }
    else if (IS_OFF_SCREEN(screen))
    {
        layer2_copy_off_screen(screen);
    }
}

static void test_blit_transparent(layer2_screen_t *screen)
{
    layer2_fill_rect(0, 0,   256, 64, 0xFE, screen);
    layer2_fill_rect(0, 64,  256, 64, 0x7E, screen);
    layer2_fill_rect(0, 128, 256, 64, 0x9F, screen);

    layer2_blit_transparent(120, 24,  sprite, 16, 16, screen); // top
    layer2_blit_transparent(120, 56,  sprite, 16, 16, screen); // top + middle
    layer2_blit_transparent(120, 88,  sprite, 16, 16, screen); // middle
    layer2_blit_transparent(120, 120, sprite, 16, 16, screen); // middle + bottom
    layer2_blit_transparent(120, 152, sprite, 16, 16, screen); // bottom
    layer2_blit_transparent(120, 184, sprite, 16, 16, screen); // bottom clipped

    layer2_blit_transparent(64, 48, tall_sprite, 2, 96, screen); // top + middle + bottom

    // Clipped blits.
    layer2_blit_transparent(248, 56,  sprite, 16, 16, screen);
    layer2_blit_transparent(248, 120, sprite, 16, 16, screen);
    layer2_blit_transparent(248, 184, sprite, 16, 16, screen);

    // Outside screen.
    layer2_blit_transparent(248, 248, sprite, 16, 16, screen);

    if (IS_SHADOW_SCREEN(screen))
    {
        layer2_flip_main_shadow_screen();
    }
    else if (IS_OFF_SCREEN(screen))
    {
        layer2_copy_off_screen(screen);
    }
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
