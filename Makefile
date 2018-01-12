################################################################################
# Stefan Bylund 2017
#
# Makefile for compiling C layer 2 screen library for Sinclair ZX Spectrum Next.
# Supports both the SCCZ80 and SDCC compilers.
################################################################################

MKDIR := mkdir -p

RM := rm -rf

ZIP := zip -r -q

SRCS := src/layer2_configure.c \
src/layer2_set_layer_priorities.c \
src/layer2_set_main_screen_ram_bank.c \
src/layer2_get_main_screen_ram_bank.c \
src/layer2_set_shadow_screen_ram_bank.c \
src/layer2_get_shadow_screen_ram_bank.c \
src/layer2_set_global_transparency_color.c \
src/layer2_get_global_transparency_color.c \
src/layer2_set_display_palette.c \
src/layer2_set_rw_palette.c \
src/layer2_set_palette.c \
src/layer2_reset_palette.c \
src/layer2_set_offset_x.c \
src/layer2_get_offset_x.c \
src/layer2_set_offset_y.c \
src/layer2_get_offset_y.c \
src/layer2_clear_screen.c \
src/layer2_load_screen.c \
src/layer2_load_palette.c \
src/layer2_copy_off_screen.c \
src/layer2_draw_pixel.c \
src/layer2_draw_pixel_fast.c \
src/layer2_draw_line.c \
src/layer2_draw_rect.c \
src/layer2_draw_text.c \
src/layer2_fill_rect.c \
src/layer2_blit.c \
src/layer2_blit_transparent.c \
src/layer2_blit_off_screen_row.c \
src/layer2_blit_off_screen_column.c \
src/layer2_blit_off_screen_sub_column.c \
src/layer2_common.c

LIBDIR_SCCZ80 := lib/sccz80

LIBDIR_SDCC_IX := lib/sdcc_ix

LIBDIR_SDCC_IY := lib/sdcc_iy

DEBUGFLAGS := --list --c-code-in-asm

all: all_sccz80 all_sdcc_ix all_sdcc_iy

all_sccz80:
	$(MKDIR) $(LIBDIR_SCCZ80)
	zcc +zx -vn -O3 -x -clib=new $(DEBUG) -Iinclude $(SRCS) -o $(LIBDIR_SCCZ80)/zxnext_layer2

all_sdcc_ix:
	$(MKDIR) $(LIBDIR_SDCC_IX)
	zcc +zx -vn -SO3 -x -clib=sdcc_ix $(DEBUG) --max-allocs-per-node200000 -Iinclude $(SRCS) -o $(LIBDIR_SDCC_IX)/zxnext_layer2

all_sdcc_iy:
	$(MKDIR) $(LIBDIR_SDCC_IY)
	zcc +zx -vn -SO3 -x -clib=sdcc_iy $(DEBUG) --max-allocs-per-node200000 -Iinclude $(SRCS) -o $(LIBDIR_SDCC_IY)/zxnext_layer2

debug_sccz80: DEBUG = $(DEBUGFLAGS)

debug_sdcc_ix: DEBUG = $(DEBUGFLAGS)

debug_sdcc_iy: DEBUG = $(DEBUGFLAGS)

debug_sccz80: all_sccz80

debug_sdcc_ix: all_sdcc_ix

debug_sdcc_iy: all_sdcc_iy

distro:
	$(RM) build/zxnext_layer2.zip
	cd ..; $(ZIP) zxnext_layer2/build/zxnext_layer2.zip zxnext_layer2/include zxnext_layer2/lib

clean:
	$(RM) lib zcc_opt.def zcc_proj.lst src/*.lis
