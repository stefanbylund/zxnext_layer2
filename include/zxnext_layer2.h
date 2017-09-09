/*******************************************************************************
 * Stefan Bylund 2017
 *
 * C API for using the layer 2 screen of the Sinclair ZX Spectrum Next as
 * specified at http://www.specnext.com/FIXME/. This C API is a thin wrapper on
 * top of the I/O port interface of the layer 2 screen. In addition, this C API
 * also provides a graphics library of drawing functions.
 *
 * The Sinclair ZX Spectrum Next provides a new graphics mode called the layer 2
 * screen. The layer 2 screen is a 256 * 192 pixels screen with 256 colours where
 * each pixel is an 8-bit RRRGGGBB colour value. The pixels are laid out linearly
 * from left to right and top to bottom. The colour pink 0xE3 (227) represents
 * the transparency colour. The colour encoding is the same as for the colour
 * palette of the hardware sprites.
 *
 * Tip: If you're drawing your screens in a general-purpose paint program, it's
 * good to know that the transparency colour 0xE3 corresponds to the 24-bit RGB
 * colour 0xE000C0 (224, 0, 192).
 *
 * The layer 2 screen can either be located behind or in front of the Spectrum
 * ULA screen. If it is in front of the ULA screen, the ULA screen will show
 * through in those pixels of the layer 2 screen that have the layer 2
 * transparency colour. If the layer 2 screen is behind the ULA screen, the
 * layer 2 screen will show through in those pixels of the ULA screen that have
 * the ULA transparency colour.
 *
 * Sidenote: The ULA screen in Sinclair ZX Spectrum Next supports four graphics
 * modes; standard Spectrum mode (256 * 192 pixels, 32 * 24 attributes, 16
 * colours), Timex high-colour mode (256 * 192 pixels, 32 * 192 attributes, 16
 * colours), Timex high-resolution mode (512 * 192 pixels in 2 colours) and
 * low-resolution mode (128 * 96 double-sized pixels in 256 colours).
 *
 * The layer 2 screen resides in a dedicated memory that is not directly
 * accessible by the Z80 CPU. In order to manipulate the layer 2 screen, it has
 * to be paged in to the Z80's 64 KB memory space. Since it would be impractical
 * to page in the whole 48 KB layer 2 screen in its entirety, it is paged-in in
 * thirds. The layer 2 screen is divided in a top, middle and bottom section of
 * the size 256 * 64 pixels and one section is paged-in at a time to the first
 * 16 KB (0-16383) of the memory space where the Spectrum BASIC ROM normally
 * resides.
 *
 * Note: The layer 2 screen is paged-in in a special way and is only accessible
 * for writing. If you read the paged-in layer 2 screen memory, you will see the
 * Spectrum BASIC ROM and not the layer 2 screen.
 *
 * This API provides a graphics library of functions for drawing on the layer 2
 * screen or on a layer 2 off-screen buffer. The drawing functions automatically
 * page in the screen/off-screen sections as necessary and take care of the
 * complications when a drawing operation covers more than one section. The
 * graphics library provides support for drawing pixels, lines, rectangles,
 * filled rectangles, text, loading of layer 2 screen files and various types of
 * blitting.
 *
 * Note: If you're drawing on a layer 2 off-screen buffer, the code to be
 * executed, the stack, the interrupt vector table and isr(s), and any required
 * data cannot be located in the RAM bank at 0xC000, which will be temporarily
 * paged out when drawing on a paged-in layer 2 off-screen buffer.
 *
 * Hardware Scrolling
 * ------------------
 *
 * The layer 2 screen supports horizontal and vertical pixel-smooth hardware
 * scrolling. The scrolling is done by offsetting the screen in the X and/or Y
 * direction in a wrapping manner. Without any offsetting applied, i.e. with the
 * X and Y offsets set to 0, the screen is made of up 256 pixel columns in the X
 * positions [0, 1, ..., 254, 255] and 192 pixel rows in the Y positions
 * [0, 1, ..., 190, 191]. The screen offsets are easiest described by the
 * examples below.
 *
 * If the X offset is set to 1, the columns of the screen will be reorganised as
 * [1, 2, ..., 254, 255, 0], setting it to 5 yields [5, 6, ..., 254, 255, 0, 1,
 * 2, 3, 4], setting it to 254 yields [254, 255, 0, 1, 2, ..., 252, 253],
 * setting it to 255 yields [255, 0, 1, 2, ..., 253, 254], and setting it back
 * to 0 yields [0, 1, ..., 254, 255]. Thus, setting the X offset to 1, 2, 3, ...,
 * 254, 255, 0 will scroll the screen, one pixel at a time, 256 pixels to the
 * left so it will be back in its original position again. Setting the X offset
 * to 255, 254, 253, ..., 2, 1, 0 will scroll the screen, one pixel at a time,
 * 256 pixels to the right so it will be back in its original position again.
 *
 * If the Y offset is set to 1, the rows of the screen will be reorganised as
 * [1, 2, ..., 190, 191, 0], setting it to 5 yields [5, 6, ..., 190, 191, 0, 1,
 * 2, 3, 4], setting it to 190 yields [190, 191, 0, 1, 2, ..., 188, 189],
 * setting it to 191 yields [191, 0, 1, 2, ..., 189, 190], and setting it back
 * to 0 yields [0, 1, ..., 190, 191]. Thus, setting the Y offset to 1, 2, 3, ...,
 * 190, 191, 0 will scroll the screen, one pixel at a time, 192 pixels upwards
 * so it will be back in its original position again. Setting the Y offset to
 * 191, 190, 189, ..., 2, 1, 0 will scroll the screen, one pixel at a time, 192
 * pixels downwards so it will be back in its original position again.
 *
 * To scroll between multiple screens horizontally, you must fill in the column
 * being scrolled out of the screen with the corresponding column from the
 * screen to be scrolled in. To scroll between multiple screens vertically, you
 * must fill in the row being scrolled out of the screen with the corresponding
 * row from the screen to be scrolled in. When scrolling between multiple
 * screens, it is convenient to use layer 2 off-screen buffers for the screens
 * to be scrolled in.
 *
 * The zxnext_layer2_demo project contains several scrolling examples that make
 * it easier to understand how the hardware scrolling of the layer 2 screen is
 * actually done.
 ******************************************************************************/

#ifndef _ZXNEXT_LAYER2_H
#define _ZXNEXT_LAYER2_H

#include <stdint.h>
#include <stdbool.h>

/* Layer 2 screen top section. */
#define LAYER2_SCREEN_TOP 0

/* Layer 2 screen middle section. */
#define LAYER2_SCREEN_MIDDLE 1

/* Layer 2 screen bottom section. */
#define LAYER2_SCREEN_BOTTOM 2

// The LAYER_PRIORITIES_* definitions are duplicated from zxnext_sprite.h.

/* Sprites over layer 2 screen over ULA screen (default). */
#ifndef LAYER_PRIORITIES_S_L_U
#define LAYER_PRIORITIES_S_L_U 0x0
#endif

/* Layer 2 screen over sprites over ULA screen. */
#ifndef LAYER_PRIORITIES_L_S_U
#define LAYER_PRIORITIES_L_S_U 0x1
#endif

/* Sprites over ULA screen over layer 2 screen. */
#ifndef LAYER_PRIORITIES_S_U_L
#define LAYER_PRIORITIES_S_U_L 0x2
#endif

/* Layer 2 screen over ULA screen over sprites. */
#ifndef LAYER_PRIORITIES_L_U_S
#define LAYER_PRIORITIES_L_U_S 0x3
#endif

/* ULA screen over sprites over layer 2 screen. */
#ifndef LAYER_PRIORITIES_U_S_L
#define LAYER_PRIORITIES_U_S_L 0x4
#endif

/* ULA screen over layer 2 screen over sprites. */
#ifndef LAYER_PRIORITIES_U_L_S
#define LAYER_PRIORITIES_U_L_S 0x5
#endif

/*
 * Macro for incrementing an Y coordinate (0-191) in a wrapping manner,
 * i.e. incrementing 191 yields 0.
 */
#define INC_Y(y) ((((y) + 1) >= 192) ? 0 : ((y) + 1))

/*
 * Macro for decrementing an Y coordinate (0-191) in a wrapping manner,
 * i.e. decrementing 0 yields 191.
 */
#define DEC_Y(y) ((((uint8_t) ((y) - 1)) == 255) ? 191 : ((y) - 1))

/*
 * Structure specifying a layer 2 off-screen buffer. A layer 2 off-screen buffer
 * is divided in a top, middle and bottom section of the size 256 * 64 pixels
 * (16 KB) in the same way as the layer 2 screen. This structure describes in
 * which RAM banks these sections reside. These RAM banks are temporarily paged-
 * in to the top 16 KB of the Z80's 64 KB memory space (i.e. at address 0xC000)
 * when drawing on the layer 2 off-screen buffer.
 *
 * Note: If you're drawing on a layer 2 off-screen buffer, the code to be
 * executed, the stack, the interrupt vector table and isr(s), and any required
 * data cannot be located in the RAM bank at 0xC000, which will be temporarily
 * paged out when drawing on a paged-in layer 2 off-screen buffer.
 */
typedef struct off_screen_buffer
{
    uint8_t top_bank;
    uint8_t middle_bank;
    uint8_t bottom_bank;
    uint8_t tmp; // Reserved
} off_screen_buffer_t;

/*
 * Configure the layer 2 screen properties. Specify if the layer 2 screen should
 * be visible, if it should be behind or in front of the Spectrum ULA screen, if
 * it should be enabled for writing, and if so, which section of the layer 2
 * screen (LAYER2_SCREEN_TOP, LAYER2_SCREEN_MIDDLE or LAYER2_SCREEN_BOTTOM)
 * should be paged in for writing to.
 */
void layer2_configure(bool layer2_visible,
                      bool layer2_behind_ula_screen,
                      bool layer2_write_enabled,
                      uint8_t layer2_screen_section);

/*
 * Set the layer priorities between the sprites and the layer 2 and ULA screens
 * (LAYER_PRIORITIES_S_L_U etc).
 */
void layer2_set_layer_priorities(uint8_t layer_priorities);

/*
 * Set the global RRRGGGBB colour used for transparency in the layer 2 screen
 * and ULA screen. The default transparency colour is 0xE3.
 *
 * The ULA screen will show through in those parts of the layer 2 screen where
 * the pixels have the specified transparency colour and vice versa.
 */
void layer2_set_global_transparency_color(uint8_t color);

/*
 * Returns the global RRRGGGBB colour used for transparency in the layer 2
 * screen and ULA screen. The default transparency colour is 0xE3.
 *
 * The ULA screen will show through in those parts of the layer 2 screen where
 * the pixels have the specified transparency colour and vice versa.
 */
uint8_t layer2_get_global_transparency_color(void);

/*
 * Offset the columns of the layer 2 screen horizontally on the X axis in a
 * wrapping manner for the specified amount of pixels (0-255).
 *
 * Passing in 1 will scroll the screen 1 pixel to the left and the column of
 * pixels being scrolled out on the left side will reappear on the right side.
 *
 * Passing in 255 will scroll the screen 1 pixel to the right and the column of
 * pixels being scrolled out on the right side will reappear on the left side.
 *
 * Passing in 1, 2, 3, ..., 254, 255, 0 will scroll the screen, one pixel at a
 * time, 256 pixels to the left so it will be back in its original position
 * again.
 *
 * Passing in 255, 254, 253, ..., 2, 1, 0 will scroll the screen, one pixel at a
 * time, 256 pixels to the right so it will be back in its original position
 * again.
 */
void layer2_set_offset_x(uint8_t offset_x);

/*
 * Return the currently set X offset value in pixels (0-255).
 */
uint8_t layer2_get_offset_x(void);

/*
 * Offset the rows of the layer 2 screen vertically on the Y axis in a wrapping
 * manner for the specified amount of pixels (0-191).
 *
 * Passing in 1 will scroll the screen 1 pixel up and the row of pixels being
 * scrolled out will reappear at the bottom.
 *
 * Passing in 191 will scroll the screen 1 pixel down and the row of pixels
 * being scrolled out will reappear at the top.
 *
 * Passing in 1, 2, 3, ..., 190, 191, 0 will scroll the screen, one pixel at a
 * time, 192 pixels upwards so it will be back in its original position again.
 *
 * Passing in 191, 190, 189, ..., 2, 1, 0 will scroll the screen, one pixel at a
 * time, 192 pixels downwards so it will be back in its original position again.
 */
void layer2_set_offset_y(uint8_t offset_y);

/*
 * Return the currently set Y offset value in pixels (0-191).
 */
uint8_t layer2_get_offset_y(void);

// Optional functions for drawing on the layer 2 screen or on a layer 2 off-
// screen buffer (paging in the screen / off-screen sections as necessary).

/*
 * Clear the layer 2 screen (or the specified layer 2 off-screen buffer) using
 * the specified RRRGGGBB colour.
 */
void layer2_clear_screen(uint8_t color, off_screen_buffer_t *off_screen_buffer);

/*
 * Load the specified layer 2 screen file (containing 256 * 192 RRRGGGBB pixels)
 * using ESXDOS into the layer 2 screen or the specified layer 2 off-screen
 * buffer.
 *
 * If there is any error when loading the file, errno is set with the
 * corresponding ESXDOS error code.
 */
void layer2_load_screen(const char *filename, off_screen_buffer_t *off_screen_buffer);

/*
 * Copy the specified layer 2 off-screen buffer to the layer 2 screen.
 */
void layer2_copy_off_screen(off_screen_buffer_t *off_screen_buffer);

/*
 * Draw a pixel on the layer 2 screen (or the specified layer 2 off-screen
 * buffer) at the point (x, y) using the specified RRRGGGBB colour.
 */
void layer2_draw_pixel(uint8_t x,
                       uint8_t y,
                       uint8_t color,
                       off_screen_buffer_t *off_screen_buffer);

/*
 * Draw a line on the layer 2 screen (or the specified layer 2 off-screen buffer)
 * between the points (x1, y1) and (x2, y2) using the specified RRRGGGBB colour.
 */
void layer2_draw_line(uint8_t x1,
                      uint8_t y1,
                      uint8_t x2,
                      uint8_t y2,
                      uint8_t color,
                      off_screen_buffer_t *off_screen_buffer);

/*
 * Draw a rectangle on the layer 2 screen (or the specified layer 2 off-screen
 * buffer) with the specified width and height using the specified RRRGGGBB
 * colour. The top-left corner of the rectangle is located at the point (x, y).
 */
void layer2_draw_rect(uint8_t x,
                      uint8_t y,
                      uint16_t width,
                      uint8_t height,
                      uint8_t color,
                      off_screen_buffer_t *off_screen_buffer);

/*
 * Draw a string of text on the layer 2 screen (or the specified layer 2 off-
 * screen buffer) at the specified row (0 - 23) and starting at the specified
 * column (0 - 31) using the specified RRRGGGBB colour. If the text doesn't fit
 * on the specified row, it is truncated at its end to fit the row.
 *
 * The text is drawn using the font set with layer2_set_font(). By default, the
 * ZX Spectrum ROM font is used. Any non-printable character outside the range
 * 32 to 127 in the ZX Spectrum character set is printed using the '?' character.
 *
 * Note that the printable characters in the ZX Spectrum character set (32 to
 * 127) are identical with ASCII except that ^, ` and DEL are replaced with
 * up-arrow, £ and (C). For example, if you want to draw the text "Price in £",
 * you use the string "Price in `" or "Price in \x60" in your program.
 */
void layer2_draw_text(uint8_t row,
                      uint8_t column,
                      const char *text,
                      uint8_t color,
                      off_screen_buffer_t *off_screen_buffer);

/*
 * Set the font to be used by layer2_draw_text(). If the specified font address
 * is NULL, the ZX Spectrum ROM font is set. By default, the ZX Spectrum ROM
 * font is used.
 *
 * The font must be a fixed-width 8 * 8 pixels font. The specified font address
 * should point to the start of the 96 printable characters in the range 32 to
 * 127 in the ZX Spectrum character set.
 */
void layer2_set_font(const void *new_font_address);

/*
 * Fill a rectangle on the layer 2 screen (or the specified layer 2 off-screen
 * buffer) with the specified width and height using the specified RRRGGGBB
 * colour. The top-left corner of the rectangle is located at the point (x, y).
 */
void layer2_fill_rect(uint8_t x,
                      uint8_t y,
                      uint16_t width,
                      uint8_t height,
                      uint8_t color,
                      off_screen_buffer_t *off_screen_buffer);

/*
 * Blit a source rectangle of the specified width (1-256) and height (1-192)
 * containing linear RRRGGGBB pixels on the layer 2 screen (or the specified
 * layer 2 off-screen buffer) at the given X (0-255) and Y (0-191) coordinates.
 */
void layer2_blit(uint8_t x,
                 uint8_t y,
                 const uint8_t *source,
                 uint16_t width,
                 uint8_t height,
                 off_screen_buffer_t *off_screen_buffer);

/*
 * Blit a source rectangle of the specified width (1-256) and height (1-192)
 * containing linear RRRGGGBB pixels on the layer 2 screen (or the specified
 * layer 2 off-screen buffer) at the given X (0-255) and Y (0-191) coordinates.
 *
 * Transparent pixels (i.e. pixels with the color 0xE3) in the source rectangle
 * are skipped.
 *
 * This blit function is useful for implementing software sprites.
 *
 * Note: In order to handle transparency, this function blits at pixel level
 * while layer2_blit() blits at line level; consequently
 * layer2_blit_transparent() is much slower than layer2_blit().
 */
void layer2_blit_transparent(uint8_t x,
                             uint8_t y,
                             const uint8_t *source,
                             uint16_t width,
                             uint8_t height,
                             off_screen_buffer_t *off_screen_buffer);

/*
 * Blit a row of pixels from the specified source layer 2 off-screen buffer at
 * the given source Y coordinate (0-191) to the layer 2 screen at the given
 * destination Y coordinate (0-191).
 *
 * This is a specialized blit function optimized for blitting new rows in layer
 * 2 screen vertical scrolling.
 */
void layer2_blit_off_screen_row(uint8_t dest_y, off_screen_buffer_t *source, uint8_t source_y);

/* Blit a column of pixels from the specified source layer 2 off-screen buffer
 * at the given source X coordinate (0-255) to the layer 2 screen at the given
 * destination X coordinate (0-255).
 *
 * This is a specialized blit function optimized for blitting new columns in
 * layer 2 screen horizontal scrolling.
 */
void layer2_blit_off_screen_column(uint8_t dest_x, off_screen_buffer_t *source, uint8_t source_x);

#endif
