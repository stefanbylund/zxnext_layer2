/*******************************************************************************
 * Stefan Bylund 2017
 *
 * A layer 2 hardware scrolling demo program for ZX Spectrum Next.
 *
 * zcc +zxn -subtype=nex -vn -SO3 -startup=30 -clib=sdcc_iy
 *   --max-allocs-per-node200000 -L<zxnext_layer2>/lib/sdcc_iy -lzxnext_layer2
 *   -I<zxnext_layer2>/include zxnext_scroll_demo.c -o zxnext_scroll_demo -create-app
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

static void test_scroll_screen_horizontally(void);

static void test_scroll_screen_vertically(void);

static void test_scroll_screen_diagonally(void);

static void test_scroll_multi_screen_horizontally(void);

static void test_scroll_multi_screen_vertically(void);

static void test_scroll_multi_screen_diagonally(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

static uint8_t test_number = 0;

static layer2_screen_t off_screen = {OFF_SCREEN, 0, 1, 3};

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
            test_scroll_screen_horizontally();
            break;
        case 1:
            test_scroll_screen_vertically();
            break;
        case 2:
            test_scroll_screen_diagonally();
            break;
        case 3:
            test_scroll_multi_screen_horizontally();
            break;
        case 4:
            test_scroll_multi_screen_vertically();
            break;
        case 5:
            test_scroll_multi_screen_diagonally();
            break;
        default:
            break;
    }

    test_number = (test_number + 1) % 6;
}

static void test_scroll_screen_horizontally(void)
{
    uint8_t offset_x = 0;
    bool increment = true;

    layer2_load_screen("screen1.nxi", NULL, 7, false);

    // 0, 1, 2, ..., 254, 255, 0, 255, 254, ..., 2, 1, 0, ...

    while (!in_inkey())
    {
        intrinsic_halt();

        if (increment)
        {
            offset_x++;
        }
        else
        {
            offset_x--;
        }

        layer2_set_offset_x(offset_x);

        if (offset_x == 0)
        {
            increment = !increment;
        }
    }

    layer2_set_offset_x(0);
}

static void test_scroll_screen_vertically(void)
{
    uint8_t offset_y = 0;
    bool increment = true;

    layer2_load_screen("screen1.nxi", NULL, 7, false);

    // 0, 1, 2, ..., 190, 191, 0, 191, 190, ..., 2, 1, 0, ...

    while (!in_inkey())
    {
        intrinsic_halt();

        if (increment)
        {
            offset_y = INC_Y(offset_y);
        }
        else
        {
            offset_y = DEC_Y(offset_y);
        }

        layer2_set_offset_y(offset_y);

        if (offset_y == 0)
        {
            increment = !increment;
        }
    }

    layer2_set_offset_y(0);
}

static void test_scroll_screen_diagonally(void)
{
    uint8_t offset_x = 0;
    uint8_t offset_y = 0;
    bool increment_x = true;
    bool increment_y = true;

    layer2_load_screen("screen1.nxi", NULL, 7, false);

    while (!in_inkey())
    {
        intrinsic_halt();

        if (increment_x)
        {
            offset_x++;
        }
        else
        {
            offset_x--;
        }

        if (increment_y)
        {
            offset_y = INC_Y(offset_y);
        }
        else
        {
            offset_y = DEC_Y(offset_y);
        }

        layer2_set_offset_x(offset_x);
        layer2_set_offset_y(offset_y);

        if (offset_x == 0)
        {
            increment_x = !increment_x;
        }

        if (offset_y == 0)
        {
            increment_y = !increment_y;
        }
    }

    layer2_set_offset_x(0);
    layer2_set_offset_y(0);
}

/*
 * Scroll horizontally between three screens numbered 0, 1 and 2 with the file
 * names wide1.nxi, wide2.nxi and wide3.nxi. The first screen file is loaded
 * into the main layer 2 screen. The screen file to be scrolled in is loaded
 * into an off-screen buffer. When the screen being scrolled in is completely
 * scrolled in, the next screen file to be scrolled in is loaded into the off-
 * screen buffer. When the last screen is completely scrolled in, the scroll
 * direction is reversed. The actual scrolling is performed in the vertical
 * blanking interval and is done by shifting the columns of the main screen by
 * one pixel and blitting the corresponding column from the off-screen buffer of
 * the screen being scrolled in.
 */
static void test_scroll_multi_screen_horizontally(void)
{
    static const char *screen_files[3] = {"wide1.nxi", "wide2.nxi", "wide3.nxi"};
    uint8_t next_screen_num = 0;
    uint8_t offset_x = 0;
    uint8_t fill_x;
    bool increment = true;

    layer2_load_screen(screen_files[next_screen_num], NULL, 7, false);

    while (!in_inkey())
    {
        if (offset_x == 0)
        {
            next_screen_num = increment ? next_screen_num + 1 : next_screen_num - 1;
            layer2_load_screen(screen_files[next_screen_num], &off_screen, 7, false);
        }

        intrinsic_halt();

        if (increment)
        {
            fill_x = offset_x;
            offset_x++;
        }
        else
        {
            offset_x--;
            fill_x = offset_x;
        }

        layer2_set_offset_x(offset_x);
        layer2_blit_off_screen_column(fill_x, &off_screen, fill_x);

        if (offset_x == 0)
        {
            if ((next_screen_num == 0) || (next_screen_num == 2))
            {
                increment = !increment;
            }
        }
    }

    layer2_set_offset_x(0);
}

/*
 * Scroll vertically between three screens numbered 0, 1 and 2 with the file
 * names tall1.nxi, tall2.nxi and tall3.nxi. The first screen file is loaded
 * into the main layer 2 screen. The screen file to be scrolled in is loaded
 * into an off-screen buffer. When the screen being scrolled in is completely
 * scrolled in, the next screen file to be scrolled in is loaded into the off-
 * screen buffer. When the last screen is completely scrolled in, the scroll
 * direction is reversed. The actual scrolling is performed in the vertical
 * blanking interval and is done by shifting the rows of the main screen by one
 * pixel and blitting the corresponding row from the off-screen buffer of the
 * screen being scrolled in.
 */
static void test_scroll_multi_screen_vertically(void)
{
    static const char *screen_files[3] = {"tall1.nxi", "tall2.nxi", "tall3.nxi"};
    uint8_t next_screen_num = 0;
    uint8_t offset_y = 0;
    uint8_t fill_y;
    bool increment = true;

    layer2_load_screen(screen_files[next_screen_num], NULL, 7, false);

    while (!in_inkey())
    {
        if (offset_y == 0)
        {
            next_screen_num = increment ? next_screen_num + 1 : next_screen_num - 1;
            layer2_load_screen(screen_files[next_screen_num], &off_screen, 7, false);
        }

        intrinsic_halt();

        if (increment)
        {
            fill_y = offset_y;
            offset_y = INC_Y(offset_y);
        }
        else
        {
            offset_y = DEC_Y(offset_y);
            fill_y = offset_y;
        }

        layer2_set_offset_y(offset_y);
        layer2_blit_off_screen_row(fill_y, &off_screen, fill_y);

        if (offset_y == 0)
        {
            if ((next_screen_num == 0) || (next_screen_num == 2))
            {
                increment = !increment;
            }
        }
    }

    layer2_set_offset_y(0);
}

/*
 * Scroll diagonally between four screens numbered 0, 1, 2 and 3 with the file
 * names diag1.nxi, diag2.nxi, diag3.nxi and diag4.nxi. The first screen file is
 * loaded into the main layer 2 screen. The screen files to be scrolled in in
 * the X direction, Y direction and the X-Y corner screen between them are
 * loaded into three off-screen buffers. When the border between the main screen
 * and one of the off-screen buffers is about to be crossed, the next screen
 * files to be scrolled in are loaded into the off-screen buffers. When the last
 * screen in the X or Y direction is completely scrolled in, that scroll
 * direction is reversed. The actual scrolling is performed in the vertical
 * blanking interval and is done by shifting the rows and columns of the main
 * screen by one pixel and blitting the corresponding row and column from the
 * off-screen buffers of the three screens being scrolled in. One part of the
 * new row is blitted from the next screen in the Y direction and the other part
 * from the X-Y corner screen. One part of the new column is blitted from the
 * next screen in the X direction and the other part from the X-Y corner screen.
 */
static void test_scroll_multi_screen_diagonally(void)
{
    static const char *screen_x[4] = {"diag3.nxi", "diag4.nxi", "diag1.nxi", "diag2.nxi"};
    static const char *screen_y[4] = {"diag2.nxi", "diag1.nxi", "diag4.nxi", "diag3.nxi"};
    static const char *screen_xy[4] = {"diag1.nxi", "diag2.nxi", "diag3.nxi", "diag4.nxi"};
    layer2_screen_t off_screen_buffer_x = {OFF_SCREEN, 0, 1, 3};
    layer2_screen_t off_screen_buffer_y = {OFF_SCREEN, 4, 6, 7};
    layer2_screen_t off_screen_buffer_xy = {OFF_SCREEN, 14, 15, 16};
    uint8_t next_screen_x = 0;
    uint8_t next_screen_y = 0;
    uint8_t offset_x = 0;
    uint8_t offset_y = 0;
    uint8_t fill_x;
    uint8_t fill_y;
    bool increment_x = true;
    bool increment_y = true;

    layer2_load_screen("diag1.nxi", NULL, 7, false);

    while (!in_inkey())
    {
        if (offset_x == 0)
        {
            next_screen_x = increment_x ? next_screen_x + 1 : next_screen_x - 1;
        }

        if (offset_y == 0)
        {
            next_screen_y = increment_y ? next_screen_y + 1 : next_screen_y - 1;
        }

        if ((offset_x == 0) || (offset_y == 0))
        {
            uint8_t i = next_screen_x + (next_screen_y << 1);
            layer2_load_screen(screen_x[i], &off_screen_buffer_x, 7, false);
            layer2_load_screen(screen_y[i], &off_screen_buffer_y, 7, false);
            layer2_load_screen(screen_xy[i], &off_screen_buffer_xy, 7, false);
        }

        intrinsic_halt();

        if (increment_x)
        {
            fill_x = offset_x;
            offset_x++;
        }
        else
        {
            offset_x--;
            fill_x = offset_x;
        }

        if (increment_y)
        {
            fill_y = offset_y;
            offset_y = INC_Y(offset_y);
        }
        else
        {
            offset_y = DEC_Y(offset_y);
            fill_y = offset_y;
        }

        layer2_set_offset_x(offset_x);
        layer2_set_offset_y(offset_y);

        // Blit row
        if (increment_x)
        {
            // Adjust the row width to 256 if offset_x is wrapping from 255 to 0.
            uint16_t row_width = ((fill_x == 255) ? 256 : offset_x);
            layer2_blit_off_screen_sub_row(offset_x, fill_y, &off_screen_buffer_y,  offset_x, fill_y, 256 - row_width);
            layer2_blit_off_screen_sub_row(0,        fill_y, &off_screen_buffer_xy, 0,        fill_y, row_width);
        }
        else
        {
            layer2_blit_off_screen_sub_row(0,        fill_y, &off_screen_buffer_y,  0,        fill_y, offset_x);
            layer2_blit_off_screen_sub_row(offset_x, fill_y, &off_screen_buffer_xy, offset_x, fill_y, 256 - offset_x);
        }

        // Blit column
        if (increment_y)
        {
            // Adjust the column height to 192 if offset_y is wrapping from 191 to 0.
            uint8_t column_height = ((fill_y == 191) ? 192 : offset_y);
            layer2_blit_off_screen_sub_column(fill_x, offset_y, &off_screen_buffer_x,  fill_x, offset_y, 192 - column_height);
            layer2_blit_off_screen_sub_column(fill_x, 0,        &off_screen_buffer_xy, fill_x, 0,        column_height);
        }
        else
        {
            layer2_blit_off_screen_sub_column(fill_x, 0,        &off_screen_buffer_x,  fill_x, 0,        offset_y);
            layer2_blit_off_screen_sub_column(fill_x, offset_y, &off_screen_buffer_xy, fill_x, offset_y, 192 - offset_y);
        }

        if (offset_x == 0)
        {
            increment_x = !increment_x;
        }

        if (offset_y == 0)
        {
            increment_y = !increment_y;
        }
    }

    layer2_set_offset_x(0);
    layer2_set_offset_y(0);
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
