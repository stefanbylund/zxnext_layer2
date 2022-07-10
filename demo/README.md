# Demonstration of zxnext_layer2

This folder contains example programs demonstrating how to use the
[zxnext_layer2](https://github.com/stefanbylund/zxnext_layer2) C layer 2
graphics library for the ZX Spectrum Next. These programs are also used for
regression testing of the zxnext_layer2 project.

This project contains four demo programs. Each demo program consists of a set of
screens, switch between the screens by pressing any key.

* zxnext_draw_demo: This program contains three sets of screens. The first set
tests all layer 2 drawing functions using the main layer 2 screen. The two other
sets are the same as the first set but all drawing is done on the shadow layer 2
screen and on a layer 2 off-screen buffer copied to the main layer 2 screen.

* zxnext_perf_demo: This program calls all layer 2 drawing functions repeatedly
to give a visual indication of their performance.

* zxnext_scroll_demo: This program demonstrates horizontal, vertical and
diagonal hardware scrolling of a single screen and between multiple screens.

* zxnext_misc_demo: This program tests layer priorities and drawing on the main
and shadow layer 2 screens paged to the top 16K RAM instead of the bottom 16K.

**Note:** This demo still works but is a bit out-of-date. It will be updated
someday when I have the time ;)

## How to Build

If you want to build the demo programs yourself, follow the steps below:

1. On Windows, you need [MinGW](http://www.mingw.org/),
[UnxUtils](https://sourceforge.net/projects/unxutils/) or
[Cygwin](https://www.cygwin.com/) for the basic Unix commands. Add the chosen
set of Unix commands to your path.

2. Install the latest version of [z88dk](https://github.com/z88dk/z88dk) and the
[ZEsarUX](https://github.com/chernandezba/zesarux) or
[CSpect](https://dailly.blogspot.se/) emulator.

3. Download the zxnext_layer2 repository either as a ZIP archive using the
"Clone or download" button at the top of this page or with Git using the
following command:

> git clone https://github.com/stefanbylund/zxnext_layer2.git

4. Go to the zxnext_layer2 directory and enter the following command to
build the C layer 2 graphics library for Spectrum Next:

> make all

5. Go to the zxnext_layer2/demo directory and enter the following command to
build the demo programs:

> make all

6. Make sure the zxnext_layer2/demo/resources directory is the root directory in
the ZEsarUX or CSpect emulator.

7. Run the zxnext_layer2/demo/bin/\<compiler-flavour\>/\<program\>.nex file in
the ZEsarUX or CSpect emulator.

**Tip:** If you don't care for makefiles or don't want to install
MinGW/UnxUtils/Cygwin on Windows, you can build zxnext_layer2 demo manually
by using the z88dk command-line invocation located in the header of the
zxnext_layer2 demo source files.

**Tip:** To start the ZEsarUX emulator directly in Spectrum Next mode, start it
with the following options:

> zesarux --noconfigfile --machine tbblue --enabletimexvideo --tbblue-fast-boot-mode
  --quickexit --enable-esxdos-handler --esxdos-root-dir \<zxnext_layer2_demo_folder\>
  \<program\>.nex

**Tip:** To start the CSpect emulator directly in Spectrum Next mode, start it
with the options below. Note that the -mmc path must end with a / or \ character!

> CSpect -w2 -tv -zxnext -mmc=\<zxnext_layer2_demo_folder\>/ \<program\>.nex

## License

This software is licensed under the terms of the MIT license.
