/*******************************************************************************
 * Stefan Bylund 2017
 *
 * C API for using the layer 2 screen of the Sinclair ZX Spectrum Next as
 * specified at http://www.specnext.com/FIXME/. This C API is a thin wrapper on
 * top of the I/O port interface of the layer 2 screen. In addition, this C API
 * also provides a graphics library of drawing functions.
 *
 * The Sinclair ZX Spectrum Next provides a new graphics mode called the layer 2
 * screen. The layer 2 screen is a 256 * 192 pixels screen with 256 colours
 * where each pixel is an 8-bit index between 0 and 255 into a 256-colour
 * palette. The pixels are laid out linearly from left to right and top to
 * bottom.
 *
 * The layer 2 palette consists of 256 9-bit RGB333 colour values, i.e. the
 * total number of colours is 512. There are actually two layer 2 palettes,
 * which one is currently used for diplaying the layer 2 screen can be selected
 * at runtime. The colour encoding of the layer 2 palette is the same as for the
 * palette of the ULA screen and hardware sprites.
 *
 * One colour is defined as the global transparency colour. This colour is an
 * 8-bit RGB332 colour value so the transparency is compared only with the 8
 * most significant bits of the 9-bit RGB333 colours in the palette. This means
 * that two of the 512 possible RGB333 colours will be transparent. By default,
 * the global transparency colour is set to the pink colour 0xE3 (227).
 *
 * Tip: If you're drawing your graphics in a general-purpose paint program, it's
 * good to know that the default global transparency colour 0xE3 corresponds to
 * the 24-bit RGB colour 0xE000C0 (224, 0, 192).
 *
 * The layer 2 screen can either be located behind or in front of the Spectrum
 * ULA screen. If it is in front of the ULA screen, the ULA screen will show
 * through in those pixels of the layer 2 screen that match the transparency
 * colour. If the layer 2 screen is behind the ULA screen, the layer 2 screen
 * will show through in those pixels of the ULA screen that match the
 * transparency colour.
 *
 * Sidenote: The ULA screen in Sinclair ZX Spectrum Next supports four graphics
 * modes; standard Spectrum mode (256 * 192 pixels, 32 * 24 attributes, 16
 * colours), Timex high-colour mode (256 * 192 pixels, 32 * 192 attributes, 16
 * colours), Timex high-resolution mode (512 * 192 pixels in 2 colours) and
 * low-resolution mode (128 * 96 double-sized pixels in 256 colours).
 *
 * There are actually two layer 2 screens: the main screen (also called the
 * front buffer) and the shadow screen (also called the back buffer). Only the
 * main screen is visible on the display. In order to manipulate the main/shadow
 * screen, it has to be paged-in to the Z80's 64 KB memory space. Since it would
 * be impractical to page-in the whole 48 KB layer 2 screen in its entirety, it
 * is paged-in in thirds. The layer 2 screen is divided in a top, middle and
 * bottom 16 KB section of the size 256 * 64 pixels and one section is paged-in
 * at a time to the first 16 KB (0 - 16383) of the memory space where the
 * Spectrum BASIC ROM normally resides. By default, the main layer 2 screen
 * resides in the extended RAM banks 8, 9 and 10 and the shadow layer 2 screen
 * in RAM banks 11, 12 and 13. Note that the layer 2 screen is paged-in in a
 * special way and is only accessible for writing. If you read the paged-in
 * screen memory, you will see the Spectrum BASIC ROM and not the layer 2 screen.
 * However, it is possible to treat the main/shadow layer 2 screen as an
 * off-screen buffer that is paged-in as any other RAM banks to the top 16 KB
 * RAM (49152 - 65535) where it is both readable and writable. When the main
 * screen is paged-in to the top 16 KB RAM instead of the bottom 16K, any writes
 * to it are still directly displayed.
 *
 * The main and shadow layer 2 screens can be flipped at anytime so that the
 * shadow screen becomes the new main screen and the old main screen becomes the
 * new shadow screen. This technique of having two screens where one is currently
 * being displayed and the other is being updated in the background and then
 * flipping them when the updates are complete is called double buffering or
 * page flipping. Using double buffering and performing the flipping between the
 * main and shadow screens in the vertical blanking interval avoids displaying
 * any intermediate updates, stutter and tearing artefacts.
 *
 * This API provides a graphics library of functions for drawing on the
 * main/shadow layer 2 screen or on a layer 2 off-screen buffer. The drawing
 * functions automatically page in the screen/off-screen sections as necessary
 * and take care of the complications when a drawing operation covers more than
 * one section. The graphics library provides support for drawing pixels, lines,
 * rectangles, filled rectangles, text, loading of layer 2 screen files and
 * various types of blitting.
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

/* Macro for determining if the given layer2_screen_t pointer is the main layer 2 screen. */
#define IS_MAIN_SCREEN(screen) (((screen) == NULL) || ((screen)->screen_type == MAIN_SCREEN))

/* Macro for determining if the given layer2_screen_t pointer is the shadow layer 2 screen. */
#define IS_SHADOW_SCREEN(screen) (((screen) != NULL) && ((screen)->screen_type == SHADOW_SCREEN))

/* Macro for determining if the given layer2_screen_t pointer is a layer 2 off-screen buffer. */
#define IS_OFF_SCREEN(screen) (((screen) != NULL) && ((screen)->screen_type == OFF_SCREEN))

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
 * Specifies the layer 2 screen type to draw on:
 *
 * MAIN_SCREEN: The main layer 2 screen paged-in to the bottom 16 KB.
 * SHADOW_SCREEN: The shadow layer 2 screen paged-in to the bottom 16 KB.
 * OFF_SCREEN: A layer 2 off-screen buffer paged-in to the top 16 KB.
 */
typedef enum layer2_screen_type
{
    MAIN_SCREEN,
    SHADOW_SCREEN,
    OFF_SCREEN
} layer2_screen_type_t;

/*
 * Structure specifying the layer 2 screen context in a drawing operation.
 * The layer 2 screen context specifies the layer 2 screen type to draw on:
 *
 * MAIN_SCREEN: The main layer 2 screen paged-in to the bottom 16 KB.
 * SHADOW_SCREEN: The shadow layer 2 screen paged-in to the bottom 16 KB.
 * OFF_SCREEN: A layer 2 off-screen buffer paged-in to the top 16 KB.
 *
 * For convenience, all drawing operations that take a pointer to a
 * layer2_screen_t as parameter will treat NULL as the main layer 2 screen.
 *
 * If the field screen_type is OFF_SCREEN, the fields top_bank, middle_bank and
 * bottom_bank specify the RAM banks for the top, middle and bottom sections of
 * the layer 2 off-screen buffer. These RAM banks are temporarily paged-in to
 * the top 16 KB of the 64 KB memory space (i.e. at address 0xC000) when drawing
 * on the layer 2 off-screen buffer. If the field screen_type is MAIN_SCREEN or
 * SHADOW_SCREEN, the fields top_bank, middle_bank and bottom_bank are not used.
 *
 * Note: If you're drawing on a layer 2 off-screen buffer, the code to be
 * executed, the stack, the interrupt vector table and isr(s), and any required
 * data cannot be located in the RAM bank at 0xC000, which will be temporarily
 * paged out when drawing on a paged-in layer 2 off-screen buffer.
 *
 * Note: The main/shadow layer 2 screen can be treated as a layer 2 off-screen
 * buffer by specifying screen_type as OFF_SCREEN and specifying the RAM banks
 * of the main/shadow layer 2 screen. The main/shadow layer 2 screen will then
 * be temporarily paged-in to the top 16 KB (instead of the bottom 16 KB) when
 * performing the drawing operation.
 */
typedef struct layer2_screen
{
    layer2_screen_type_t screen_type;
    uint8_t top_bank;
    uint8_t middle_bank;
    uint8_t bottom_bank;
    uint8_t tmp; // Reserved
} layer2_screen_t;

/*
 * Configure the layer 2 screen properties. Specify if the layer 2 screen should
 * be visible or not. You can also specify if the layer 2 screen should be
 * enabled for writing, and if so, if the main or shadow layer 2 screen should
 * be written to and which section of the specified layer 2 screen
 * (LAYER2_SCREEN_TOP, LAYER2_SCREEN_MIDDLE or LAYER2_SCREEN_BOTTOM) should be
 * paged in for writing.
 *
 * If you use the graphics functions in this library, you only need to initially
 * configure the layer 2 screen to be visible, i.e. layer2_configure(true, false,
 * false, 0), the graphics functions will internally handle the paging and
 * writing of the main or shadow layer 2 screen. However, if you want to write
 * directly on the layer 2 screen, you need to call this function to specify
 * where to write.
 */
void layer2_configure(bool layer2_visible,
                      bool layer2_write_enabled,
                      bool layer2_shadow_screen,
                      uint8_t layer2_screen_section);

/*
 * Set the layer priorities between the sprites and the layer 2 and ULA screens
 * (LAYER_PRIORITIES_S_L_U etc).
 */
void layer2_set_layer_priorities(uint8_t layer_priorities);

/*
 * Set the main layer 2 screen RAM banks. The given parameter specifies the RAM
 * bank for the top section of the main layer 2 screen. The middle and bottom
 * sections implicitly follows on the consecutive RAM banks.
 *
 * By default, the RAM banks 8 (top section), 9 (middle section) and 10 (bottom
 * section) are used by the main layer 2 screen.
 *
 * Note: This function determines which layer 2 screen RAM banks are actually
 * displayed on the screen. Calling this function with the starting RAM bank of
 * the layer 2 shadow screen or a layer 2 off-screen buffer (with consecutive
 * RAM banks) will make the layer 2 shadow screen or the layer 2 off-screen
 * buffer the new main layer 2 screen and their contents will be directly
 * visible on the display.
 */
void layer2_set_main_screen_ram_bank(uint8_t bank);

/*
 * Returns the starting RAM bank of the main layer 2 screen. The returned value
 * specifies the RAM bank for the top section of the main layer 2 screen. The
 * middle and bottom sections implicitly follows on the consecutive RAM banks.
 *
 * By default, the RAM banks 8 (top section), 9 (middle section) and 10 (bottom
 * section) are used by the main layer 2 screen.
 */
uint8_t layer2_get_main_screen_ram_bank(void);

/*
 * Set the shadow layer 2 screen RAM banks. The given parameter specifies the
 * RAM bank for the top section of the shadow layer 2 screen. The middle and
 * bottom sections implicitly follows on the consecutive RAM banks.
 *
 * By default, the RAM banks 11 (top section), 12 (middle section) and 13
 * (bottom section) are used by the shadow layer 2 screen.
 */
void layer2_set_shadow_screen_ram_bank(uint8_t bank);

/*
 * Returns the starting RAM bank of the shadow layer 2 screen. The returned
 * value specifies the RAM bank for the top section of the shadow layer 2
 * screen. The middle and bottom sections implicitly follows on the consecutive
 * RAM banks.
 *
 * By default, the RAM banks 11 (top section), 12 (middle section) and 13
 * (bottom section) are used by the shadow layer 2 screen.
 */
uint8_t layer2_get_shadow_screen_ram_bank(void);

/*
 * Set the global RGB332 colour used for transparency in the layer 2 screen and
 * ULA screen. The default transparency colour is 0xE3.
 *
 * The ULA screen will show through in those parts of the layer 2 screen where
 * the pixels match the specified transparency colour and vice versa.
 */
void layer2_set_global_transparency_color(uint8_t color);

/*
 * Returns the global RGB332 colour used for transparency in the layer 2 screen
 * and ULA screen. The default transparency colour is 0xE3.
 *
 * The ULA screen will show through in those parts of the layer 2 screen where
 * the pixels match the specified transparency colour and vice versa.
 */
uint8_t layer2_get_global_transparency_color(void);

/*
 * Set the layer 2 display palette (first or second palette), i.e. the palette
 * used when displaying the layer 2 screen. By default, the first palette is
 * used.
 */
void layer2_set_display_palette(bool first_palette);

/*
 * Set the layer 2 read/write palette (first or second palette), i.e. the
 * palette used when reading/writing the layer 2 palette colours.
 */
void layer2_set_rw_palette(bool first_palette);

/*
 * Set a range of RGB333 colours starting from the given palette index in the
 * currently selected layer 2 read/write palette.
 *
 * The RGB333 colours are 16-bit values where the high byte holds the RGB332
 * bits (RRRGGGBB) and the low byte the zero-extended lowest blue bit (0000000B).
 */
void layer2_set_palette(const uint16_t *colors, uint16_t length, uint8_t palette_index);

/*
 * Offset the columns of the main layer 2 screen horizontally on the X axis in a
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
 * Offset the rows of the main layer 2 screen vertically on the Y axis in a
 * wrapping manner for the specified amount of pixels (0-191).
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

// Optional functions for drawing on the main/shadow layer 2 screen or on a layer
// 2 off-screen buffer (paging in the screen / off-screen sections as necessary).

/*
 * Clear the specified layer 2 screen using the colour of the specified palette
 * index.
 */
void layer2_clear_screen(uint8_t color, layer2_screen_t *screen);

/*
 * Load the specified layer 2 screen file (containing 256 * 192 8-bit pixels)
 * using ESXDOS into the specified layer 2 screen memory.
 *
 * If the has_palette parameter is true, it is assumed that the layer 2 screen
 * file is prepended with a 256-colour RGB333 palette (512 bytes long), which is
 * loaded and whose colours are set in the currently selected layer 2 read/write
 * palette. The RGB333 colours should be 16-bit values where the first byte
 * holds the RGB332 bits (RRRGGGBB) and the second byte the zero-extended lowest
 * blue bit (0000000B).
 *
 * If there is any error when loading the file, errno is set with the
 * corresponding ESXDOS error code.
 */
void layer2_load_screen(const char *filename, layer2_screen_t *screen, bool has_palette);

/*
 * Load the specified layer 2 palette file (containing 256 RGB333 colours, 512
 * bytes in size) using ESXDOS into the currently selected layer 2 read/write
 * palette.
 *
 * The RGB333 colours should be 16-bit values where the first byte holds the
 * RGB332 bits (RRRGGGBB) and the second byte the zero-extended lowest blue bit
 * (0000000B).
 *
 * If there is any error when loading the file, errno is set with the
 * corresponding ESXDOS error code.
 */
void layer2_load_palette(const char *filename);

/*
 * Copy the specified layer 2 off-screen buffer to the main layer 2 screen.
 */
void layer2_copy_off_screen(layer2_screen_t *off_screen_buffer);

/*
 * Draw a pixel on the specified layer 2 screen at the point (x, y) using the
 * colour of the specified palette index.
 */
void layer2_draw_pixel(uint8_t x,
                       uint8_t y,
                       uint8_t color,
                       layer2_screen_t *screen);

/*
 * Draw a line on the specified layer 2 screen between the points (x1, y1) and
 * (x2, y2) using the colour of the specified palette index.
 */
void layer2_draw_line(uint8_t x1,
                      uint8_t y1,
                      uint8_t x2,
                      uint8_t y2,
                      uint8_t color,
                      layer2_screen_t *screen);

/*
 * Draw a rectangle on the specified layer 2 screen with the specified width
 * and height using the colour of the specified palette index. The top-left
 * corner of the rectangle is located at the point (x, y).
 */
void layer2_draw_rect(uint8_t x,
                      uint8_t y,
                      uint16_t width,
                      uint8_t height,
                      uint8_t color,
                      layer2_screen_t *screen);

/*
 * Draw a string of text on the specified layer 2 screen at the specified row
 * (0 - 23) and starting at the specified column (0 - 31) using the colour of
 * the specified palette index If the text doesn't fit on the specified row, it
 * is truncated at its end to fit the row.
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
                      layer2_screen_t *screen);

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
 * Fill a rectangle on the specified layer 2 screen with the specified width
 * and height using the colour of the specified palette index. The top-left
 * corner of the rectangle is located at the point (x, y).
 */
void layer2_fill_rect(uint8_t x,
                      uint8_t y,
                      uint16_t width,
                      uint8_t height,
                      uint8_t color,
                      layer2_screen_t *screen);

/*
 * Blit a source rectangle of the specified width (1-256) and height (1-192)
 * containing linear pixels on the specified layer 2 screen at the given X
 * (0-255) and Y (0-191) coordinates.
 */
void layer2_blit(uint8_t x,
                 uint8_t y,
                 const uint8_t *source,
                 uint16_t width,
                 uint8_t height,
                 layer2_screen_t *screen);

/*
 * Blit a source rectangle of the specified width (1-256) and height (1-192)
 * containing linear pixels on the specified layer 2 screen at the given X
 * (0-255) and Y (0-191) coordinates.
 *
 * Transparent pixels (i.e. pixels matching the defined global transparency
 * color) in the source rectangle are skipped.
 *
 * This blit function is useful for implementing software sprites.
 *
 * Note: In order to handle transparency, this function blits at pixel level
 * while layer2_blit() blits at line level; consequently
 * layer2_blit_transparent() is much slower than layer2_blit().
 *
 * Currently, this function only works correctly with the default palette and
 * default global transparency colour.
 */
void layer2_blit_transparent(uint8_t x,
                             uint8_t y,
                             const uint8_t *source,
                             uint16_t width,
                             uint8_t height,
                             layer2_screen_t *screen);

/*
 * Blit a full row of pixels from the specified source layer 2 off-screen buffer
 * at the given source Y coordinate (0-191) to the main layer 2 screen at the
 * given destination Y coordinate (0-191).
 *
 * This is a specialized blit function optimized for blitting new rows in layer
 * 2 screen vertical scrolling.
 */
#define layer2_blit_off_screen_row(dest_y, source, source_y) \
    layer2_blit_off_screen_sub_row(0, (dest_y), (source), 0, (source_y), 256)

/*
 * Blit a sub-row of pixels of the given width from the specified source layer 2
 * off-screen buffer at the given source Y coordinate (0-191) to the main layer
 * 2 screen at the given destination Y coordinate (0-191).
 *
 * This is a specialized blit function optimized for blitting new sub-rows in
 * the vertical part of layer 2 screen diagonal scrolling.
 */
void layer2_blit_off_screen_sub_row(uint8_t dest_x,
                                    uint8_t dest_y,
                                    layer2_screen_t *source,
                                    uint8_t source_x,
                                    uint8_t source_y,
                                    uint16_t width);

/* Blit a full column of pixels from the specified source layer 2 off-screen
 * buffer at the given source X coordinate (0-255) to the main layer 2 screen
 * at the given destination X coordinate (0-255).
 *
 * This is a specialized blit function optimized for blitting new columns in
 * layer 2 screen horizontal scrolling.
 */
void layer2_blit_off_screen_column(uint8_t dest_x, layer2_screen_t *source, uint8_t source_x);

/* Blit a sub-column of pixels of the given height from the specified source
 * layer 2 off-screen buffer at the given source X coordinate (0-255) to the
 * main layer 2 screen at the given destination X coordinate (0-255).
 *
 * This is a specialized blit function optimized for blitting new sub-columns in
 * the horizontal part of layer 2 screen diagonal scrolling.
 */
void layer2_blit_off_screen_sub_column(uint8_t dest_x,
                                       uint8_t dest_y,
                                       layer2_screen_t *source,
                                       uint8_t source_x,
                                       uint8_t source_y,
                                       uint8_t height);

#endif
