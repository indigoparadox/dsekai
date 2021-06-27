
DSEKAI_C_FILES := \
   src/tilemap.c \
   src/graphics.c \
   src/mobile.c \
   src/item.c \
   src/window.c \
   src/topdown.c

DSEKAI_C_FILES_SDL := \
   src/input/sdli.c \
   src/graphics/sdlg.c \
   src/main.c \
   src/data/drc.c

DSEKAI_C_FILES_DOS := \
   src/input/dosi.c \
   src/graphics/dosg.c \
   src/main.c \
   src/data/drc.c

DSEKAI_C_FILES_PALM := \
   src/input/palmi.c \
   src/graphics/palmg.c \
   src/mainpalm.c \
   src/data/dio.c

DSEKAI_C_FILES_WIN16 := src/input/win16i.c src/graphics/win16g.c src/mainw16.c

DSEKAI_C_FILES_MAC7 := \
   src/input/mac7i.c \
   src/graphics/mac7g.c \
   src/main.c

DSEKAI_C_FILES_CHECK := \
   check/check.c \
   check/check_mobile.c \
   check/check_item.c \
   check/check_tilemap.c \
   check/check_window.c \
   check/check_graphics.c \
   check/check_engines.c \
   check/check_data.c \
   src/graphics/nullg.c \
   src/input/nulli.c \
   tools/data/cga.c \
   tools/data/bmp.c \
   tools/data/json.c \
   src/data/dio.c

MKRESH_C_FILES := \
   tools/mkresh.c \
   src/data/dio.c

DRCPACK_C_FILES := \
   tools/drcpack.c \
   tools/data/drcwrite.c \
   src/data/drc.c \
   src/data/dio.c

CONVERT_C_FILES := \
   tools/convert.c \
   tools/data/bmp.c \
   src/data/drc.c \
   tools/data/cga.c \
   src/data/dio.c \
   tools/data/header.c \
   tools/data/json.c

LOOKUPS_C_FILES: tools/lookups.c

TOPDOWN_O: src/topdown.o

ASSETDIR := assets

OBJDIR_SDL := obj/sdl/
OBJDIR_DOS := obj/dos/
OBJDIR_PALM := obj/palm/
OBJDIR_WIN16 := obj/win16/
OBJDIR_MAC7 := obj/mac7/
OBJDIR_CHECK_NULL := obj/check_null/

GENDIR_SDL := gen/sdl
GENDIR_DOS := gen/dos
GENDIR_PALM := gen/palm
GENDIR_WIN16 := gen/win16
GENDIR_MAC7 := gen/mac7

BINDIR := bin

BIN_SDL := $(BINDIR)/dsekai
BIN_DOS := $(BINDIR)/dsekai.exe
BIN_PALM := $(BINDIR)/dsekai.prc
BIN_WIN16 := $(BINDIR)/dsekai16.exe
BIN_MAC7 := $(BINDIR)/dsekai.bin $(BINDIR)/dsekai.APPL $(BINDIR)/dsekai.dsk

BIN_CHECK_NULL := $(BINDIR)/check

DSEKAI_ASSETS_SPRITES := $(wildcard $(ASSETDIR)/sprite_*.bmp)
DSEKAI_ASSETS_TILES := $(wildcard $(ASSETDIR)/tile_*.bmp)
DSEKAI_ASSETS_PATTERNS := $(wildcard $(ASSETDIR)/pattern_*.bmp)
DSEKAI_ASSETS_BITMAPS := \
   $(DSEKAI_ASSETS_SPRITES) \
   $(DSEKAI_ASSETS_TILES) \
   $(DSEKAI_ASSETS_PATTERNS)
DSEKAI_ASSETS_MASKS := $(wildcard $(ASSETDIR)/mask_*.bmp)
DSEKAI_ASSETS_MASKS_H := \
   $(subst assets/,gen/,$(subst .bmp,.h,$(DSEKAI_ASSETS_MASKS)))
DSEKAI_ASSETS_DOS_CGA := \
   $(subst .bmp,.cga,$(subst $(ASSETDIR)/,$(GENDIR_DOS)/,$(DSEKAI_ASSETS_BITMAPS)))
DSEKAI_ASSETS_PALM := \
   $(subst $(ASSETDIR)/,$(GENDIR_PALM)/,$(DSEKAI_ASSETS_BITMAPS))

MD := mkdir -p
MKRESH := bin/mkresh
DRCPACK := bin/drcpack
CONVERT := bin/convert
LOOKUPS := bin/lookups

CFLAGS_MKRESH := -DMEMORY_CALLOC -DNO_RESEXT -g
CFLAGS_DRCPACK := -DMEMORY_CALLOC -DNO_RESEXT -g -DDRC_READ_WRITE
CFLAGS_CONVERT := -DMEMORY_CALLOC -DNO_RESEXT -g
CFLAGS_LOOKUPS := -g

CFLAGS_DEBUG_GCC := -Wall -Wno-missing-braces -Wno-char-subscripts -fsanitize=address -fsanitize=leak

$(BIN_SDL): CFLAGS := -DSCREEN_SCALE=3 $(shell pkg-config sdl2 --cflags) -g -DSCREEN_W=160 -DSCREEN_H=160 -std=c89 -DPLATFORM_SDL -DDIO_SILENT -DMEMORY_CALLOC $(CFLAGS_DEBUG_GCC)
$(BIN_SDL): LDFLAGS := $(shell pkg-config sdl2 --libs) -g $(CFLAGS_DEBUG_GCC)

$(BIN_DOS): CC := wcc
$(BIN_DOS): LD := wcl
$(BIN_DOS): CFLAGS := -hw -d3 -0 -mm -DSCALE_2X -DPLATFORM_DOS -DDIO_SILENT -DMEMORY_CALLOC -DUSE_LOOKUPS
$(BIN_DOS): LDFLAGS := $(CFLAGS)

$(BIN_PALM): CC := m68k-palmos-gcc
$(BIN_PALM): PILRC := pilrc
$(BIN_PALM): TXT2BITM := txt2bitm
$(BIN_PALM): OBJRES := m68k-palmos-obj-res
$(BIN_PALM): BUILDPRC := build-prc
$(BIN_PALM): INCLUDES := -I /opt/palmdev/sdk-3.5/include -I /opt/palmdev/sdk-3.5/include/Core/UI/ -I /opt/palmdev/sdk-3.5/include/Core/System/ -I /opt/palmdev/sdk-3.5/include/Core/Hardware/ -I /opt/palmdev/sdk-3.5/include/Core/International/
$(BIN_PALM): CFLAGS := -O0 -DSCREEN_W=160 -DSCREEN_H=160 $(INCLUDES) -DHIDE_WELCOME_DIALOG -DNO_PALM_DEBUG_LINE -DDISABLE_FILESYSTEM -DPLATFORM_PALM -g -DMEMORY_STATIC -DIGNORE_DIRTY
$(BIN_PALM): LDFLAGS = -g
$(BIN_PALM): ICONTEXT := "dsekai"
$(BIN_PALM): APPID := DSEK
$(BIN_PALM): PALMS_RCP := src/palms.rcp

$(BIN_WIN16): CC := wcc
$(BIN_WIN16): LD := wcl
$(BIN_WIN16): RC := wrc
$(BIN_WIN16): CFLAGS := -bt=windows -i=$(INCLUDE)/win -DSCALE_2X -DUSE_LOOKUPS -DPLATFORM_WIN16 -DMEMORY_CALLOC
$(BIN_WIN16): LDFLAGS := -l=windows

$(BIN_MAC7): CC := m68k-apple-macos-gcc
$(BIN_MAC7): CFLAGS := -DPLATFORM_MAC7 -DMEMORY_CALLOC -DDIO_SILENT
$(BIN_MAC7): REZ := Rez
$(BIN_MAC7): REZFLAGS :=
$(BIN_MAC7): RETRO68_PREFIX := /opt/Retro68-build/toolchain

$(BIN_CHECK_NULL): CFLAGS := -DSCREEN_SCALE=3 $(shell pkg-config check --cflags) -g -DSCREEN_W=160 -DSCREEN_H=160 -std=c89 -DPLATFORM_NULL -DMEMORY_CALLOC -DDIO_SILENTL $(CFLAGS_DEBUG_GCC)
$(BIN_CHECK_NULL): LDFLAGS := $(shell pkg-config check --libs) -g $(CFLAGS_DEBUG_GCC)

DSEKAI_O_FILES_SDL := \
   $(addprefix $(OBJDIR_SDL),$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_SDL),$(subst .c,.o,$(DSEKAI_C_FILES_SDL)))
DSEKAI_O_FILES_DOS := \
   $(addprefix $(OBJDIR_DOS),$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_DOS),$(subst .c,.o,$(DSEKAI_C_FILES_DOS)))
DSEKAI_O_FILES_PALM := \
   $(addprefix $(OBJDIR_PALM),$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_PALM),$(subst .c,.o,$(DSEKAI_C_FILES_PALM)))
DSEKAI_O_FILES_WIN16 := \
   $(addprefix $(OBJDIR_WIN16),$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_WIN16),$(subst .c,.o,$(DSEKAI_C_FILES_WIN16)))
DSEKAI_O_FILES_MAC7 := \
   $(addprefix $(OBJDIR_MAC7),$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_MAC7),$(subst .c,.o,$(DSEKAI_C_FILES_MAC7)))
DSEKAI_O_FILES_CHECK_NULL := \
   $(addprefix $(OBJDIR_CHECK_NULL),$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_CHECK_NULL),$(subst .c,.o,$(DSEKAI_C_FILES_CHECK)))

.PHONY: clean res_sdl16_drc res_doscga_drc res_palm grc_palm res_masks

all: $(BIN_DOS) $(BIN_SDL) $(BIN_PALM)

$(BINDIR):
	$(MD) $(BINDIR)

gen/mask_%.h: assets/mask_%.bmp $(CONVERT)
	$(CONVERT) -ic bitmap -oc header -if $< -of $@ -ob 1

res_masks: $(DSEKAI_ASSETS_MASKS_H)

# ====== Utilities ======

$(MKRESH): $(MKRESH_C_FILES) | $(BINDIR)
	gcc $(CFLAGS_MKRESH) -o $@ $^

$(DRCPACK): $(DRCPACK_C_FILES) | $(BINDIR)
	gcc $(CFLAGS_DRCPACK) -o $@ $^

$(CONVERT): $(CONVERT_C_FILES) | $(BINDIR)
	gcc $(CFLAGS_CONVERT) -o $@ $^

$(LOOKUPS): $(LOOKUPS_C_FILES) | $(BINDIR)
	gcc $(CFLAGS_LOOKUPS) -o $@ tools/lookups.c

# ====== Main: SDL ======

$(GENDIR_SDL):
	$(MD) $@

res_sdl16_drc: $(DRCPACK) | $(GENDIR_SDL)
	$(DRCPACK) -c -a -af $(BINDIR)/sdl16.drc -t BMP1 -i 5001 \
      -if $(DSEKAI_ASSETS_BITMAPS) -lh $(GENDIR_SDL)/resext.h

$(BINDIR)/sdl16.drc: res_sdl16_drc

$(BIN_SDL): $(DSEKAI_O_FILES_SDL) | $(BINDIR)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJDIR_SDL)%.o: %.c res_sdl16_drc res_masks
	$(MD) $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $(<:%.o=%)

# ====== Main: MS-DOS ======

$(BINDIR)/doscga.drc: res_doscga_drc

res_doscga_drc: $(DRCPACK) $(DSEKAI_ASSETS_DOS_CGA)
	$(DRCPACK) -c -a -af $(BINDIR)/doscga.drc -t BMP1 -i 5001 \
      -if $(GENDIR_DOS)/*.cga -lh $(GENDIR_DOS)/resext.h

$(GENDIR_DOS):
	$(MD) $@

$(GENDIR_DOS)/%.cga: $(ASSETDIR)/%.bmp $(CONVERT) | $(GENDIR_DOS)
	$(CONVERT) -ic bitmap -oc cga -ob 2 -if $< -of $@ -og

$(BIN_DOS): $(DSEKAI_O_FILES_DOS)
	$(MD) $(BINDIR)
	$(LD) $(LDFLAGS) -fe=$@ $^

$(OBJDIR_DOS)%.o: %.c res_doscga_drc res_masks
	$(MD) $(dir $@)
	$(CC) $(CFLAGS) -fo=$@ $(<:%.c=%)

# ====== Main: Palm ======

$(GENDIR_PALM):
	$(MD) $@

$(GENDIR_PALM)/%.bmp: $(ASSETDIR)/%.bmp $(CONVERT) | $(GENDIR_PALM)
	$(CONVERT) -if $< -of $@ -ob 1 -r -ic bitmap -oc bitmap

res_palm: $(DSEKAI_ASSETS_PALM)

rcp_h_palm: res_palm $(MKRESH) | $(GENDIR_PALM)
	$(MKRESH) -f palm -i 5001 -if $(GENDIR_PALM)/*.bmp -oh $(GENDIR_PALM)/resext.h -or $(GENDIR_PALM)/palmd.rcp

$(GENDIR_PALM)/resext.h: rcp_h_palm

$(GENDIR_PALM)/palmd.rcp: rcp_h_palm

grc_palm: $(OBJDIR_PALM)dsekai
	cd $(OBJDIR_PALM) && $(OBJRES) dsekai

$(OBJDIR_PALM)dsekai: $(DSEKAI_O_FILES_PALM)
	$(CC) $(CFLAGS) $^ -o $@
	
$(OBJDIR_PALM)bin.stamp: src/palms.rcp $(GENDIR_PALM)/palmd.rcp
	$(PILRC) $< $(OBJDIR_PALM)
	touch $@

$(BIN_PALM): grc_palm $(OBJDIR_PALM)bin.stamp | $(BINDIR)
	$(BUILDPRC) $@ $(ICONTEXT) $(APPID) $(OBJDIR_PALM)*.grc $(OBJDIR_PALM)*.bin $(LINKFILES) 

$(OBJDIR_PALM)%.o: %.c res_palm $(GENDIR_PALM)/resext.h
	$(MD) $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $(<:%.o=%)

# ====== Main: Win16 ======

$(GENDIR_WIN16):
	$(MD) $@

$(BIN_WIN16): $(DSEKAI_O_FILES_WIN16) $(OBJDIR_WIN16)win16.res | $(BINDIR)
	$(LD) $(LDFLAGS) -fe=$@ $^

$(OBJDIR_WIN16)$(TOPDOWN_O): $(RESEXT_H)

$(OBJDIR_WIN16)win16.res: $(GENDIR_WIN16)/win16.rc
	$(RC) -r -i=$(INCLUDE)/win src/win16s.rc -fo=$@

$(GENDIR_WIN16)/win16.rc: $(DSEKAI_ASSETS_BITMAPS) $(MKRESH) | $(GENDIR_WIN16)
	$(MKRESH) -f win16 -i 5001 -if $(DSEKAI_ASSETS_BITMAPS) -oh $(GENDIR_WIN16)/resext.h -or $@

$(OBJDIR_WIN16)%.o: %.c $(OBJDIR_WIN16)win16.res
	$(MD) $(dir $@)
	$(CC) $(CFLAGS) -fo=$@ $(<:%.c=%)

# ====== Main: MacOS 7 ======

$(OBJDIR_MAC7)dsekai.code.bin: $(DSEKAI_O_FILES_MAC7)
	$(MD) $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $^

$(BIN_MAC7): $(OBJDIR_MAC7)dsekai.code.bin
	$(REZ) $(REZFLAGS) \
      --copy "$^" \
      "$(RETRO68_PREFIX)/RIncludes/Retro68APPL.r" \
      -t "APPL" -c "DSEK" \
      -o $(BINDIR)/dsekai.bin \
      --cc $(BINDIR)/dsekai.APPL \
      --cc $(BIN_MAC7)

$(OBJDIR_MAC7)%.o: %.c
	$(MD) $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $(<:%.o=%)

# ====== Check: Null ======

$(BIN_CHECK_NULL): $(DSEKAI_O_FILES_CHECK_NULL) | $(BINDIR)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJDIR_CHECK_NULL)%.o: %.c res_sdl16_drc
	$(MD) $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $(<:%.o=%)

# ====== Clean ======

clean:
	rm -rf data obj bin gen *.err

