# C Layer 2 Screen API for Sinclair ZX Spectrum Next

The **zxnext_layer2** project provides a C API for using the layer 2 screen of
the Sinclair ZX Spectrum Next as specified at http://www.specnext.com/FIXME/.
This API is a thin C wrapper on top of the I/O port interface of the layer 2
screen system. In addition, this C API also provides a graphics library of
drawing functions.

The [zxnext_layer2_demo](https://github.com/stefanbylund/zxnext_layer2_demo)
project contains an example program demonstrating how to use this API.

## Disclaimer

This API is a work in progress and will be continuously updated as more
information about the layer 2 screen is made known and implemented by the
ZEsarUX and CSpect emulators and when the layer 2 screen specification is
finally published.

This API has been tested and verified to work with the z88dk C compiler
(2017-08-27 snapshot) and the ZEsarUX 5.1 (2017-07-24 snapshot) and CSpect 0.9
emulators. See the "Known Problems" section below for a list of known problems
and workarounds.

## Download

The latest version of this API can be downloaded [here](build/zxnext_layer2.zip).
This download contains the following header files and libraries:

* zxnext_layer2/include/zxnext_layer2.h
* zxnext_layer2/include/zxnext_registers.h
* zxnext_layer2/lib/sccz80/zxnext_layer2.lib
* zxnext_layer2/lib/sdcc_ix/zxnext_layer2.lib
* zxnext_layer2/lib/sdcc_iy/zxnext_layer2.lib

If you want to build the zxnext_layer2 libraries yourself, see the "How to build"
section below.

## API Documentation

The zxnext_layer2 API is documented in the following header files:

* [zxnext_layer2.h](include/zxnext_layer2.h)
* [zxnext_registers.h](include/zxnext_registers.h)

## How to Use

1. Download [zxnext_layer2.zip](build/zxnext_layer2.zip) and unpack it in a
suitable place. It contains the files listed in the "Download" section above.

2. Install the latest version of [z88dk](https://github.com/z88dk/z88dk) and
the [ZEsarUX](https://sourceforge.net/projects/zesarux/) or
[CSpect](https://dailly.blogspot.se/) emulator.

4. Read about how the layer 2 screen work in the "Layer 2 Screen" section below
or in the official specification at http://www.specnext.com/FIXME/.

5. Familiarize yourself with the zxnext_layer2.h API.

6. Include zxnext_layer2.h in your program and start creating layer 2 graphics.

7. Compile your program with z88dk and link it with the appropriate version of
zxnext_layer2.lib.

8. Run your program in the ZEsarUX or CSpect emulator.

**Note:** Since the Sinclair ZX Spectrum Next is still under development and all
tools need to catch up with the latest specs, it is important to use the latest
version of z88dk and ZEsarUX or CSpect.

**Tip:** See the [zxnext_layer2_demo](https://github.com/stefanbylund/zxnext_layer2_demo)
project for an example of how to use zxnext_layer2.h and link with zxnext_layer2.lib.

**Tip:** There are two C compilers in z88dk; the SDCC compiler and the SCCZ80
compiler. I recommend to use the SDCC compiler since it generates more optimised
code than the SCCZ80 compiler. There are two versions of the C runtime library
for the SDCC compiler; one called sdcc_ix and another called sdcc_iy. From my
experience, sdcc_iy is the more stable version. In summary, for optimal
performance, use the SDCC compiler with the sdcc_iy C runtime library. However,
the SCCZ80 compiler still has it use during development since it compiles much
faster than the SDCC compiler.

**Tip:** If you copy the zxnext_layer2 header files and libraries into the
following directories in z88dk, the z88dk compiler will automatically find them
without the need for setting up any include and library paths:

* z88dk/include/_DEVELOPMENT/sccz80/
* z88dk/include/_DEVELOPMENT/sdcc/
* z88dk/libsrc/_DEVELOPMENT/lib/sccz80/
* z88dk/libsrc/_DEVELOPMENT/lib/sdcc_ix/
* z88dk/libsrc/_DEVELOPMENT/lib/sdcc_iy/

**Tip:** To start the ZEsarUX emulator directly in Sinclair ZX Spectrum Next
mode, start it with the following options:

> zesarux --machine tbblue --enable-mmc --enable-divmmc-ports --mmc-file extras/media/disk_images/tbblue.mmc
    --enable-esxdos-handler --esxdos-root-dir extras/media/spectrum/esxdos_handler/0.8.6_TBBLUE --quickexit

**Tip:** To start the CSpect emulator directly in Sinclair ZX Spectrum Next
mode, start it with the following options:

> CSpect -zxnext -s7 -mmc=<virtual_mmc_root_folder> <my_program>.sna

## How to Build

If you want to build the zxnext_layer2 libraries yourself, follow the steps below:

1. On Windows, you need [MinGW](http://www.mingw.org/),
[UnxUtils](https://sourceforge.net/projects/unxutils/) or
[Cygwin](https://www.cygwin.com/) for the basic Unix commands. Add the chosen
set of Unix commands to your path.

2. Install the latest version of [z88dk](https://github.com/z88dk/z88dk) and add
it to your path.

3. Download the zxnext_layer2 repository either as a ZIP archive using the
"Clone or download" button at the top of this page or with Git using the
following command:

> git clone https://github.com/stefanbylund/zxnext_layer2.git

4. Go to the zxnext_layer2 repository and enter the following command:

> make all

## Layer 2 Screen

The Sinclair ZX Spectrum Next provides a new graphics mode called the layer 2
screen. The layer 2 screen is a 256 * 192 pixels screen with 256 colours where
each pixel is an 8-bit RRRGGGBB colour value. The pixels are laid out linearly
from left to right and top to bottom. The colour pink 0xE3 (227) represents
the transparency colour. The colour encoding is the same as for the colour
palette of the hardware sprites.

Tip: If you're drawing your screens in a general-purpose paint program, it's
good to know that the transparency colour 0xE3 corresponds to the 24-bit RGB
colour 0xE000C0 (224, 0, 192).

The layer 2 screen can either be located behind or in front of the Spectrum
ULA screen. If it is in front of the ULA screen, the ULA screen will show
through in those pixels of the layer 2 screen that have the layer 2
transparency colour. If the layer 2 screen is behind the ULA screen, the
layer 2 screen will show through in those pixels of the ULA screen that have
the ULA transparency colour.

Sidenote: The ULA screen in Sinclair ZX Spectrum Next supports four graphics
modes; standard Spectrum mode (256 * 192 pixels, 32 * 24 attributes, 16
colours), Timex high-colour mode (256 * 192 pixels, 32 * 192 attributes, 16
colours), Timex high-resolution mode (512 * 192 pixels in 2 colours) and
low-resolution mode (128 * 96 double-sized pixels in 256 colours).

The layer 2 screen resides in a dedicated memory that is not directly
accessible by the Z80 CPU. In order to manipulate the layer 2 screen, it has
to be paged in to the Z80's 64 KB memory space. Since it would be impractical
to page in the whole 48 KB layer 2 screen in its entirety, it is paged-in in
thirds. The layer 2 screen is divided in a top, middle and bottom section of
the size 256 * 64 pixels and one section is paged-in at a time to the first
16 KB (0-16383) of the memory space where the Spectrum BASIC ROM normally
resides.

Note: The layer 2 screen is paged-in in a special way and is only accessible
for writing. If you read the paged-in layer 2 screen memory, you will see the
Spectrum BASIC ROM and not the layer 2 screen.

This API provides a graphics library of functions for drawing on the layer 2
screen or on a layer 2 off-screen buffer. The drawing functions automatically
page in the screen/off-screen sections as necessary and take care of the
complications when a drawing operation covers more than one section. The
graphics library provides support for drawing pixels, lines, rectangles,
filled rectangles, text, loading of layer 2 screen files and various types of
blitting.

Note: If you're drawing on a layer 2 off-screen buffer, the code to be
executed, the stack, the interrupt vector table and isr(s), and any required
data cannot be located in the RAM bank at 0xC000, which will be temporarily
paged out when drawing on a paged-in layer 2 off-screen buffer.

### Hardware Scrolling

The layer 2 screen supports horizontal and vertical pixel-smooth hardware
scrolling. The scrolling is done by offsetting the screen in the X and/or Y
direction in a wrapping manner. Without any offsetting applied, i.e. with the
X and Y offsets set to 0, the screen is made of up 256 pixel columns in the X
positions [0, 1, ..., 254, 255] and 192 pixel rows in the Y positions
[0, 1, ..., 190, 191]. The screen offsets are easiest described by the
examples below.

If the X offset is set to 1, the columns of the screen will be reorganised as
[1, 2, ..., 254, 255, 0], setting it to 5 yields [5, 6, ..., 254, 255, 0, 1,
2, 3, 4], setting it to 254 yields [254, 255, 0, 1, 2, ..., 252, 253],
setting it to 255 yields [255, 0, 1, 2, ..., 253, 254], and setting it back
to 0 yields [0, 1, ..., 254, 255]. Thus, setting the X offset to 1, 2, 3, ...,
254, 255, 0 will scroll the screen, one pixel at a time, 256 pixels to the
left so it will be back in its original position again. Setting the X offset
to 255, 254, 253, ..., 2, 1, 0 will scroll the screen, one pixel at a time,
256 pixels to the right so it will be back in its original position again.

If the Y offset is set to 1, the rows of the screen will be reorganised as
[1, 2, ..., 190, 191, 0], setting it to 5 yields [5, 6, ..., 190, 191, 0, 1,
2, 3, 4], setting it to 190 yields [190, 191, 0, 1, 2, ..., 188, 189],
setting it to 191 yields [191, 0, 1, 2, ..., 189, 190], and setting it back
to 0 yields [0, 1, ..., 190, 191]. Thus, setting the Y offset to 1, 2, 3, ...,
190, 191, 0 will scroll the screen, one pixel at a time, 192 pixels upwards
so it will be back in its original position again. Setting the Y offset to
191, 190, 189, ..., 2, 1, 0 will scroll the screen, one pixel at a time, 192
pixels downwards so it will be back in its original position again.

To scroll between multiple screens horizontally, you must fill in the column
being scrolled out of the screen with the corresponding column from the
screen to be scrolled in. To scroll between multiple screens vertically, you
must fill in the row being scrolled out of the screen with the corresponding
row from the screen to be scrolled in. When scrolling between multiple
screens, it is convenient to use layer 2 off-screen buffers for the screens
to be scrolled in.

The [zxnext_layer2_demo](https://github.com/stefanbylund/zxnext_layer2_demo)
project contains several scrolling examples that make it easier to understand
how the hardware scrolling of the layer 2 screen is actually done.

## Known Problems

Hardware scrolling of the layer 2 screen is currently not supported by ZEsarUX.

When using the ZEsarUX emulator, the layer2_draw_text() function, if using the
Spectrum ROM font, only works if the TAP file is given as a command-line argument
to ZEsarUX, not if loaded from its GUI. If layer2_draw_text() uses the Spectrum
ROM font (which is the default), it pages in ROM bank 1 which contains the
Spectrum ROM font. When the TAP file is loaded from the ZEsarUX GUI, the attempt
to page in the Spectrum BASIC ROM to the bottom 16 KB fails for an unknown
reason.

When using the CSpect emulator, the sccz80 flavour of the layer2_load_screen()
function does not work. The sdcc_ix and sdcc_iy flavours work fine though. The
sccz80 flavour can be made to work in CSpect by removing a temporary workaround
in layer2_load_screen() but then it will stop working in ZEsarUX. At the next
update of ZEsarUX, this workaround will fortunately no longer be needed.

## License

This software is licensed under the terms of the MIT license.
