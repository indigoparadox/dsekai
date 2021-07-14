
DSEKAI_C_FILES := \
   src/tilemap.c \
   src/graphics.c \
   src/mobile.c \
   src/item.c \
   src/window.c \
   src/dio.c \
   src/control.c \
   src/topdown.c

DSEKAI_C_FILES_SDL_ONLY := \
   src/main.c \
   src/json.c \
   src/input/sdli.c \
   src/graphics/sdlg.c \
   src/memory/fakem.c \
   src/resource/header.c

DSEKAI_C_FILES_DOS_ONLY := \
   src/main.c \
   src/input/dosi.c \
   src/graphics/dosg.c \
   src/memory/fakem.c \
   src/resource/header.c

DSEKAI_C_FILES_PALM_ONLY := \
   src/main.c \
   src/json.c \
   src/input/palmi.c \
   src/memory/palmm.c \
   src/resource/palmr.c \
   src/graphics/palmg.c

DSEKAI_C_FILES_WIN_ONLY := \
   src/main.c \
   src/input/wini.c \
   src/resource/winr.c \
   src/memory/winm.c \
   src/graphics/wing.c

DSEKAI_C_FILES_MAC7_ONLY := \
   src/main.c \
   src/input/mac7i.c \
   src/graphics/mac7g.c \
   src/memory/mac7m.c \
   src/resource/header.c

DSEKAI_C_FILES_NDS_ONLY := \
   src/main.c \
   src/input/ndsi.c \
   src/graphics/ndsg.c \
   src/memory/fakem.c \
   src/resource/header.c

DSEKAI_C_FILES_CHECK_NULL_ONLY := \
   src/json.c \
   check/check.c \
   check/ckmobile.c \
   check/ckitem.c \
   check/cktmap.c \
   check/ckwindow.c \
   check/ckgfx.c \
   check/cktopdwn.c \
   check/ckdataim.c \
   check/ckdatajs.c \
   src/resource/drcr.c \
   check/ckdio.c \
   check/ckdrc.c \
   check/ckmemory.c \
   src/graphics/nullg.c \
   src/input/nulli.c \
   tools/data/cga.c \
   tools/data/bmp.c \
   tools/data/icns.c \
   tools/data/drcwrite.c \
   src/memory/fakem.c \
   src/drc.c \
   src/dio.c

MKRESH_C_FILES := \
   tools/mkresh.c \
   src/memory/fakem.c \
   src/drc.c \
   src/dio.c

DRCPACK_C_FILES := \
   tools/drcpack.c \
   tools/data/drcwrite.c \
   src/memory/fakem.c \
   src/drc.c \
   src/dio.c

CONVERT_C_FILES := \
   tools/convert.c \
   tools/data/bmp.c \
   src/memory/fakem.c \
   src/drc.c \
   tools/data/cga.c \
   src/dio.c \
   tools/data/icns.c \
   src/json.c

LOOKUPS_C_FILES: tools/lookups.c

TOPDOWN_O: src/topdown.o

ASSETDIR := assets

OBJDIR_SDL :=        obj/sdl
OBJDIR_DOS :=        obj/dos
OBJDIR_PALM :=       obj/palm
OBJDIR_WIN16 :=      obj/win16
OBJDIR_WIN32 :=      obj/win32
OBJDIR_MAC7 :=       obj/mac7
OBJDIR_NDS :=        obj/nds
OBJDIR_CHECK_NULL := obj/check_null

DEPDIR_SDL :=        dep/sdl
DEPDIR_DOS :=        dep/dos
DEPDIR_PALM :=       dep/palm
DEPDIR_WIN16 :=      dep/win16
DEPDIR_WIN32 :=      dep/win32
DEPDIR_MAC7 :=       dep/mac7
DEPDIR_NDS :=        dep/nds
DEPDIR_CHECK_NULL := dep/check_null

GENDIR_SDL := gen/sdl
GENDIR_DOS := gen/dos
GENDIR_PALM := gen/palm
GENDIR_WIN16 := gen/win16
GENDIR_WIN32 := gen/win32
GENDIR_MAC7 := gen/mac7
GENDIR_NDS := gen/nds
GENDIR_CHECK_NULL := gen/check_null

BINDIR := bin

BIN_SDL := $(BINDIR)/dsekai
BIN_DOS := $(BINDIR)/dsekai.exe
BIN_PALM := $(BINDIR)/dsekai.prc
BIN_WIN16 := $(BINDIR)/dsekai16.exe
BIN_WIN32 := $(BINDIR)/dsekai32.exe
BIN_MAC7 := $(BINDIR)/dsekai.bin $(BINDIR)/dsekai.APPL $(BINDIR)/dsekai.dsk
BIN_NDS := $(BINDIR)/dsekai.nds

BIN_CHECK_NULL := $(BINDIR)/check

DSEKAI_ASSETS_SPRITES := $(wildcard $(ASSETDIR)/sprite_*.bmp)
DSEKAI_ASSETS_TILES := $(wildcard $(ASSETDIR)/tile_*.bmp)
DSEKAI_ASSETS_PATTERNS := $(wildcard $(ASSETDIR)/pattern_*.bmp)
DSEKAI_ASSETS_BITMAPS := \
   $(DSEKAI_ASSETS_SPRITES) \
   $(DSEKAI_ASSETS_TILES) \
   $(DSEKAI_ASSETS_PATTERNS)
DSEKAI_ASSETS_MAPS := \
   $(ASSETDIR)/map_field.json
DSEKAI_ASSETS_DOS_CGA := \
   $(subst .bmp,.cga,$(subst $(ASSETDIR)/,$(GENDIR_DOS)/,$(DSEKAI_ASSETS_BITMAPS)))
DSEKAI_ASSETS_PALM := \
   $(subst $(ASSETDIR)/,$(GENDIR_PALM)/,$(DSEKAI_ASSETS_BITMAPS))
DSEKAI_ASSETS_ICNS := \
   $(subst .bmp,.icns,$(subst $(ASSETDIR)/,$(GENDIR_MAC7)/,$(DSEKAI_ASSETS_BITMAPS)))
DSEKAI_ASSETS_RSRC := \
   $(subst .bmp,.rsrc,$(subst $(ASSETDIR)/,$(GENDIR_MAC7)/,$(DSEKAI_ASSETS_BITMAPS)))
DSEKAI_ASSETS_PICTS := \
   $(subst .bmp,.pict,$(subst $(ASSETDIR)/,$(GENDIR_MAC7)/,$(DSEKAI_ASSETS_BITMAPS)))
DSEKAI_ASSETS_MAPS_WIN16 := \
   $(subst .json,.h,$(subst $(ASSETDIR)/,$(GENDIR_WIN16)/,$(DSEKAI_ASSETS_MAPS)))
DSEKAI_ASSETS_MAPS_WIN32 := \
   $(subst .json,.h,$(subst $(ASSETDIR)/,$(GENDIR_WIN32)/,$(DSEKAI_ASSETS_MAPS)))
DSEKAI_ASSETS_MAPS_MAC7 := \
   $(subst .json,.h,$(subst $(ASSETDIR)/,$(GENDIR_MAC7)/,$(DSEKAI_ASSETS_MAPS)))
DSEKAI_ASSETS_MAPS_NDS := \
   $(subst .json,.h,$(subst $(ASSETDIR)/,$(GENDIR_NDS)/,$(DSEKAI_ASSETS_MAPS)))
DSEKAI_ASSETS_MAPS_DOS := \
   $(subst .json,.h,$(subst $(ASSETDIR)/,$(GENDIR_DOS)/,$(DSEKAI_ASSETS_MAPS)))

MD := mkdir -p
DD := /bin/dd
MCOPY := mcopy
MKFSVFAT := /sbin/mkfs.vfat
IMAGEMAGICK := convert
PYTHON := python3

MAPC := scripts/mapc.py

MKRESH := bin/mkresh
DRCPACK := bin/drcpack
CONVERT := bin/convert
LOOKUPS := bin/lookups
HEADPACK := bin/headpack

RETRO68_PREFIX := /opt/Retro68-build/toolchain

CFLAGS_MKRESH := -DNO_RESEXT -g -DDEBUG_LOG -DDEBUG_THRESHOLD=0 -DUSE_JSON_MAPS -DRESOURCE_DRC
CFLAGS_DRCPACK := -DNO_RESEXT -g -DDRC_READ_WRITE -DDEBUG_LOG -DDEBUG_THRESHOLD=3 -DUSE_JSON_MAPS -DRESOURCE_DRC
CFLAGS_CONVERT := -DNO_RESEXT -g -DUSE_JSON_MAPS -DRESOURCE_DRC
CFLAGS_LOOKUPS := -g -DRESOURCE_HEADER
CFLAGS_HEADPACK := -g -DRESOURCE_HEADER

CFLAGS_DEBUG_GENERIC := -DDEBUG_LOG -DDEBUG_THRESHOLD=2
CFLAGS_DEBUG_GCC := $(CFLAGS_DEBUG_GENERIC) -Wall -Wno-missing-braces -Wno-char-subscripts -fsanitize=address -fsanitize=leak -fsanitize=undefined -pg

CFLAGS_SDL := -DSCREEN_SCALE=3 $(shell pkg-config sdl2 --cflags) -g -DSCREEN_W=160 -DSCREEN_H=160 -std=c89 -DPLATFORM_SDL $(CFLAGS_DEBUG_GCC)
CFLAGS_DOS := -hw -d3 -0 -ms -DPLATFORM_DOS -DUSE_LOOKUPS -zp=1 -DDEBUG_THRESHOLD=1
CFLAGS_PALM := -Os -DSCREEN_W=160 -DSCREEN_H=160 $(INCLUDES) -DPLATFORM_PALM -g $(CFLAGS_DEBUG_GENERIC)
CFLAGS_WIN16 := -bt=windows -i=$(INCLUDE)/win -bw -DSCREEN_SCALE=2 -DPLATFORM_WIN16 $(CFLAGS_DEBUG_GENERIC) -zp=1
CFLAGS_WIN32 := -bt=nt -3 -i=$(INCLUDE) -i=$(INCLUDE)/nt -DSCREEN_SCALE=2 -DPLATFORM_WIN32 $(CFLAGS_DEBUG_GENERIC) -zp=1
CFLAGS_MAC7 := -DPLATFORM_MAC7 -I$(RETRO68_PREFIX)/multiversal/CIncludes $(CFLAGS_DEBUG_GENERIC)
CFLAGS_NDS := --sysroot $(DEVKITARM)/arm-none-eabi -I$(DEVKITPRO)/libnds/include -DPLATFORM_NDS -DARM9 -g -march=armv5te -mtune=arm946e-s -fomit-frame-pointer -ffast-math
CFLAGS_CHECK_NULL := -DSCREEN_SCALE=3 $(shell pkg-config check --cflags) -g -DSCREEN_W=160 -DSCREEN_H=160 -std=c89 -DPLATFORM_NULL $(CFLAGS_DEBUG_GCC) -DRESOURCE_DRC

$(BIN_SDL): LDFLAGS := $(shell pkg-config sdl2 --libs) -g $(CFLAGS_DEBUG_GCC)

$(BIN_DOS): CC := wcc
$(BIN_DOS): LD := wcl
$(BIN_DOS): LDFLAGS := $(CFLAGS_DOS)

$(BIN_PALM): CC := m68k-palmos-gcc
$(BIN_PALM): PILRC := pilrc
$(BIN_PALM): TXT2BITM := txt2bitm
$(BIN_PALM): OBJRES := m68k-palmos-obj-res
$(BIN_PALM): BUILDPRC := build-prc
$(BIN_PALM): INCLUDES := -I /opt/palmdev/sdk-3.5/include -I /opt/palmdev/sdk-3.5/include/Core/UI/ -I /opt/palmdev/sdk-3.5/include/Core/System/ -I /opt/palmdev/sdk-3.5/include/Core/Hardware/ -I /opt/palmdev/sdk-3.5/include/Core/International/
$(BIN_PALM): LDFLAGS = -g
$(BIN_PALM): ICONTEXT := "dsekai"
$(BIN_PALM): APPID := DSEK
$(BIN_PALM): PALMS_RCP := src/palms.rcp

$(BIN_WIN16): CC := wcc
$(BIN_WIN16): LD := wcl
$(BIN_WIN16): RC := wrc
$(BIN_WIN16): LDFLAGS := -l=windows -zp=1

$(BIN_WIN32): CC := wcc386
$(BIN_WIN32): LD := wcl386
$(BIN_WIN32): RC := wrc
$(BIN_WIN32): LDFLAGS := -bcl=nt_win -zp=1

$(BIN_MAC7): CC := m68k-apple-macos-gcc
$(BIN_MAC7): CXX := m68k-apple-macos-g++
$(BIN_MAC7): LDFLAGS := -lRetroConsole
$(BIN_MAC7): REZ := Rez
$(BIN_MAC7): REZFLAGS :=

$(BIN_NDS): CC := arm-none-eabi-gcc
$(BIN_NDS): LD := arm-none-eabi-gcc
$(BIN_NDS): ARCH := -mthumb -mthumb-interwork
$(BIN_NDS): DEVKITPATH := $(shell echo "$(DEVKITPRO)" | sed -e 's/^\([a-zA-Z]\):/\/\1/')
$(BIN_NDS): PATH := $(DEVKITPATH)/tools/bin:$(DEVKITPATH)/devkitARM/bin:$(PATH)
$(BIN_NDS): CFLAGS_NDS += $(ARCH)
$(BIN_NDS): LDFLAGS := -specs=ds_arm9.specs -g $(ARCH) -Wl,-Map,$(OBJDIR_NDS)/dsekai.map
$(BIN_NDS): LIBS := -L$(DEVKITPRO)/libnds/lib -lnds9

$(BIN_CHECK_NULL): LDFLAGS := $(shell pkg-config check --libs) -g $(CFLAGS_DEBUG_GCC)

DSEKAI_C_FILES_CHECK_NULL := $(DSEKAI_C_FILES) $(DSEKAI_C_FILES_CHECK_NULL_ONLY)

DSEKAI_O_FILES_SDL := \
   $(addprefix $(OBJDIR_SDL)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_SDL)/,$(subst .c,.o,$(DSEKAI_C_FILES_SDL_ONLY)))
DSEKAI_O_FILES_DOS := \
   $(addprefix $(OBJDIR_DOS)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_DOS)/,$(subst .c,.o,$(DSEKAI_C_FILES_DOS_ONLY)))
DSEKAI_O_FILES_PALM := \
   $(addprefix $(OBJDIR_PALM)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_PALM)/,$(subst .c,.o,$(DSEKAI_C_FILES_PALM_ONLY)))
DSEKAI_O_FILES_WIN16 := \
   $(addprefix $(OBJDIR_WIN16)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_WIN16)/,$(subst .c,.o,$(DSEKAI_C_FILES_WIN_ONLY)))
DSEKAI_O_FILES_WIN32 := \
   $(addprefix $(OBJDIR_WIN32)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_WIN32)/,$(subst .c,.o,$(DSEKAI_C_FILES_WIN_ONLY)))
DSEKAI_O_FILES_MAC7 := \
   $(addprefix $(OBJDIR_MAC7)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_MAC7)/,$(subst .c,.o,$(DSEKAI_C_FILES_MAC7_ONLY)))
DSEKAI_O_FILES_NDS := \
   $(addprefix $(OBJDIR_NDS)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_NDS)/,$(subst .c,.o,$(DSEKAI_C_FILES_NDS_ONLY)))
DSEKAI_O_FILES_CHECK_NULL := \
   $(addprefix $(OBJDIR_CHECK_NULL)/,$(subst .c,.o,$(DSEKAI_C_FILES_CHECK_NULL)))

.PHONY: clean res_sdl16_drc res_doscga_drc res_palm grc_palm res_mac7

all: $(BIN_DOS) $(BIN_SDL) $(BIN_PALM)

$(BINDIR):
	$(MD) $(BINDIR)

# ====== Utilities ======

$(MKRESH): $(MKRESH_C_FILES) | $(BINDIR)
	gcc $(CFLAGS_MKRESH) -o $@ $^

$(DRCPACK): $(DRCPACK_C_FILES) | $(BINDIR)
	gcc $(CFLAGS_DRCPACK) -o $@ $^

$(CONVERT): $(CONVERT_C_FILES) | $(BINDIR)
	gcc $(CFLAGS_CONVERT) -o $@ $^

$(LOOKUPS): $(LOOKUPS_C_FILES) | $(BINDIR)
	gcc $(CFLAGS_LOOKUPS) -o $@ tools/lookups.c

$(HEADPACK): tools/headpack.c | $(BINDIR)
	gcc $(CFLAGS_HEADPACK) -o $@ tools/headpack.c

# ====== Main: SDL ======

$(GENDIR_SDL):
	$(MD) $@

#res_sdl16_drc: $(DRCPACK) | $(GENDIR_SDL)
#	rm $(BINDIR)/sdl16.drc || true
#	$(DRCPACK) -c -a -af $(BINDIR)/sdl16.drc -i 5001 \
#      -if $(DSEKAI_ASSETS_BITMAPS) $(DSEKAI_ASSETS_MAPS) \
#      -lh $(GENDIR_SDL)/resext.h

#$(BINDIR)/sdl16.drc: 

$(GENDIR_SDL)/resext.h: \
$(HEADPACK) $(DSEKAI_ASSETS_BITMAPS) $(DSEKAI_ASSETS_MAPS) | $(GENDIR_SDL)
	$(HEADPACK) $@ $(DSEKAI_ASSETS_BITMAPS) $(DSEKAI_ASSETS_MAPS)

$(BIN_SDL): $(DSEKAI_O_FILES_SDL) | $(BINDIR)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJDIR_SDL)/src/topdown.o: $(DSEKAI_ASSETS_MAPS)

$(OBJDIR_SDL)/%.o: %.c $(GENDIR_SDL)/resext.h
	$(MD) $(dir $@)
	$(CC) $(CFLAGS_SDL) -c -o $@ $(<:%.o=%)

$(DEPDIR_SDL)/%.d: %.c $(GENDIR_SDL)/resext.h
	$(MD) $(dir $@)
	$(CC) $(CFLAGS_SDL) -MM $< \
      -MT $(subst .c,.o,$(addprefix $(DEPDIR_SDL)/,$<)) -MF $@

include $(subst obj/,dep/,$(DSEKAI_O_FILES_SDL:.o=.d))

# ====== Main: MS-DOS ======

#$(BINDIR)/doscga.drc: res_doscga_drc

#res_doscga_drc: $(DRCPACK) $(DSEKAI_ASSETS_DOS_CGA)
#	rm $(BINDIR)/doscga.drc || true
#	$(DRCPACK) -c -a -af $(BINDIR)/doscga.drc -i 5001 \
#      -if $(GENDIR_DOS)/*.cga $(DSEKAI_ASSETS_MAPS) -lh $(GENDIR_DOS)/resext.h

$(GENDIR_DOS)/map_%.h: $(ASSETDIR)/map_%.json $(MAPC) | $(GENDIR_DOS)
	$(PYTHON) $(MAPC) -j $< -o $@

$(GENDIR_DOS)/resext.h: \
$(HEADPACK) $(DSEKAI_ASSETS_DOS_CGA) $(DSEKAI_ASSETS_MAPS)
	$(HEADPACK) $@ $(DSEKAI_ASSETS_DOS_CGA) $(DSEKAI_ASSETS_MAPS)

$(GENDIR_DOS):
	$(MD) $@

$(GENDIR_DOS)/%.cga: $(ASSETDIR)/%.bmp $(CONVERT) | $(GENDIR_DOS)
	$(CONVERT) -ic bitmap -oc cga -ob 2 -if $< -of $@ -og

$(BIN_DOS): $(DSEKAI_O_FILES_DOS) | $(BINDIR) $(DSEKAI_ASSETS_MAPS_DOS)
	$(LD) $(LDFLAGS) -fe=$@ $^

$(OBJDIR_DOS)/%.o: %.c $(GENDIR_DOS)/resext.h $(DSEKAI_ASSETS_MAPS_DOS)
	$(MD) $(dir $@)
	$(CC) $(CFLAGS_DOS) -fo=$@ $(<:%.c=%)

# ====== Main: Palm ======

$(GENDIR_PALM):
	$(MD) $@

$(GENDIR_PALM)/%.bmp: $(ASSETDIR)/%.bmp $(CONVERT) | $(GENDIR_PALM)
	$(CONVERT) -if $< -of $@ -ob 1 -r -ic bitmap -oc bitmap

res_palm: $(DSEKAI_ASSETS_PALM)

rcp_h_palm: res_palm $(MKRESH) | $(GENDIR_PALM)
	$(MKRESH) -f palm -i 5001 \
      -if $(DSEKAI_ASSETS_PALM) $(DSEKAI_ASSETS_MAPS) \
      -oh $(GENDIR_PALM)/resext.h \
      -or $(GENDIR_PALM)/palmd.rcp

$(GENDIR_PALM)/resext.h: rcp_h_palm

$(GENDIR_PALM)/palmd.rcp: rcp_h_palm

grc_palm: $(OBJDIR_PALM)/dsekai
	cd $(OBJDIR_PALM) && $(OBJRES) dsekai

$(OBJDIR_PALM)/dsekai: $(DSEKAI_O_FILES_PALM)
	$(CC) $(CFLAGS_PALM) $^ -o $@
	
$(OBJDIR_PALM)/bin.stamp: src/palms.rcp $(GENDIR_PALM)/palmd.rcp
	$(PILRC) $< $(OBJDIR_PALM)
	touch $@

$(BIN_PALM): grc_palm $(OBJDIR_PALM)/bin.stamp | $(BINDIR)
	$(BUILDPRC) $@ $(ICONTEXT) $(APPID) \
      $(OBJDIR_PALM)/*.grc $(OBJDIR_PALM)/*.bin $(LINKFILES) 

$(OBJDIR_PALM)/%.o: %.c res_palm $(GENDIR_PALM)/resext.h
	$(MD) $(dir $@)
	$(CC) $(CFLAGS_PALM) -c -o $@ $(<:%.o=%)

# ====== Main: Win16 ======

$(GENDIR_WIN16):
	$(MD) $@

$(OBJDIR_WIN16):
	$(MD) $@

$(GENDIR_WIN16)/%.ico: $(ASSETDIR)/%.bmp | $(GENDIR_WIN16)
	$(IMAGEMAGICK) $< $@

$(GENDIR_WIN16)/%.h: $(ASSETDIR)/%.json $(MAPC) | $(GENDIR_WIN16)
	$(PYTHON) $(MAPC) -j $< -o $@

$(BINDIR)/dsekai16.img: $(BIN_WIN16)
	$(DD) if=/dev/zero bs=512 count=2880 of="$@"
	$(MKFSVFAT) "$@"
	$(MCOPY) -i "$@" $< ::dsekai16.exe

$(BIN_WIN16): \
$(DSEKAI_O_FILES_WIN16) $(OBJDIR_WIN16)/win16.res | $(BINDIR) $(DSEKAI_ASSETS_MAPS_WIN16)
	$(LD) $(LDFLAGS) -fe=$@ $^

$(OBJDIR_WIN16)/win16.res: \
$(GENDIR_WIN16)/win16.rc $(ASSETDIR)/dsekai.ico | $(OBJDIR_WIN16)
	$(RC) -r -DPLATFORM_WIN16 -i $(INCLUDE)win src/winstat.rc -o $@

$(GENDIR_WIN16)/win16.rc \
$(GENDIR_WIN16)/resext.h: $(DSEKAI_ASSETS_BITMAPS) $(MKRESH) | $(GENDIR_WIN16)
	$(MKRESH) -f win16 -i 5001 \
      -if $(DSEKAI_ASSETS_BITMAPS) $(DSEKAI_ASSETS_MAPS) \
      -oh $(GENDIR_WIN16)/resext.h -or $(GENDIR_WIN16)/win16.rc

$(OBJDIR_WIN16)/%.o: \
%.c $(OBJDIR_WIN16)/win16.res $(GENDIR_WIN16)/resext.h $(DSEKAI_ASSETS_MAPS_WIN16)
	$(MD) $(dir $@)
	$(CC) $(CFLAGS_WIN16) -fo=$@ $(<:%.c=%)

# ====== Main: Win32 ======

$(GENDIR_WIN32):
	$(MD) $@

$(OBJDIR_WIN32):
	$(MD) $@

$(GENDIR_WIN32)/map_%.h: $(ASSETDIR)/map_%.json $(MAPC) | $(GENDIR_WIN32)
	$(PYTHON) $(MAPC) -j $< -o $@

$(BIN_WIN32): \
$(DSEKAI_O_FILES_WIN32) $(OBJDIR_WIN32)/win32.res | \
$(BINDIR) $(DSEKAI_ASSETS_MAPS_WIN32)
	$(LD) $(LDFLAGS) -fe=$@ $^

$(OBJDIR_WIN32)/win32.res: \
$(GENDIR_WIN32)/win32.rc $(ASSETDIR)/dsekai.ico | $(OBJDIR_WIN32)
	$(RC) -r -DPLATFORM_WIN32 -i $(INCLUDE)win src/winstat.rc -o $@

$(GENDIR_WIN32)/win32.rc \
$(GENDIR_WIN32)/resext.h: $(DSEKAI_ASSETS_BITMAPS) $(MKRESH) | $(GENDIR_WIN32)
	$(MKRESH) -f win16 -i 5001 \
      -if $(DSEKAI_ASSETS_BITMAPS) $(DSEKAI_ASSETS_MAPS) \
      -oh $(GENDIR_WIN32)/resext.h -or $(GENDIR_WIN32)/win32.rc

$(OBJDIR_WIN32)/%.o: \
%.c $(OBJDIR_WIN32)/win32.res $(GENDIR_WIN32)/resext.h $(DSEKAI_ASSETS_MAPS_WIN32)
	$(MD) $(dir $@)
	$(CC) $(CFLAGS_WIN32) -fo=$@ $(<:%.c=%)

# ====== Main: MacOS 7 ======

$(OBJDIR_MAC7):
	$(MD) $@

$(GENDIR_MAC7):
	$(MD) $@

$(GENDIR_MAC7)/%.h: $(ASSETDIR)/%.json $(MAPC) | $(GENDIR_MAC7)
	$(PYTHON) $(MAPC) -j $< -o $@

#$(GENDIR_MAC7)/resext.h: $(GENDIR_MAC7) $(MKRESH)
#	$(MKRESH) -f palm -i 5001 \
#      -if $(DSEKAI_ASSETS_ICNS) \
#      -oh $(GENDIR_MAC7)/resext.h \

#$(BINDIR)/mac7.drc \
#$(GENDIR_MAC7)/resext.h: $(DSEKAI_ASSETS_PICTS) $(DRCPACK) | $(GENDIR_SDL)
#	$(DRCPACK) -c -a -af $(BINDIR)/mac7.drc -t PICT -i 5001 \
#      -if $(DSEKAI_ASSETS_PICTS) $(DSEKAI_ASSETS_MAPS) \
#      -lh $(GENDIR_MAC7)/resext.h

$(GENDIR_MAC7)/resext.h: \
$(HEADPACK) $(DSEKAI_ASSETS_PICTS) $(DSEKAI_ASSETS_MAPS) | $(GENDIR_MAC7)
	$(HEADPACK) $@ $(DSEKAI_ASSETS_PICTS) $(DSEKAI_ASSETS_MAPS)

$(GENDIR_MAC7)/%.pict: $(ASSETDIR)/%.bmp | $(GENDIR_MAC7)
	$(IMAGEMAGICK) $< $@

#$(GENDIR_MAC7)/%.rsrc: $(GENDIR_MAC7)/%.icns
#	echo "read 'icns' (-16455) \"$<\";" > $@

$(GENDIR_MAC7)/%.icns: $(ASSETDIR)/%.bmp $(CONVERT) | $(GENDIR_MAC7)
	$(CONVERT) -if $< -of $@ -ob 1 -r -ic bitmap -oc icns

$(OBJDIR_MAC7)/dsekai.code.bin: $(DSEKAI_O_FILES_MAC7) | $(OBJDIR_MAC7)
	$(CXX) $(LDFLAGS) -o $@ $^ # Use CXX to link for RetroConsole.

$(BIN_MAC7): $(OBJDIR_MAC7)/dsekai.code.bin
	$(REZ) -I$(RETRO68_PREFIX)/RIncludes \
      --copy $^ \
      "$(RETRO68_PREFIX)/RIncludes/Retro68APPL.r" \
      -t "APPL" -c "DSEK" \
      -o $(BINDIR)/dsekai.bin \
      --cc $(BINDIR)/dsekai.APPL \
      --cc $(BINDIR)/dsekai.dsk

$(OBJDIR_MAC7)/%.o: \
%.c $(DSEKAI_ASSETS_MAPS_MAC7) $(GENDIR_MAC7)/resext.h | $(OBJDIR_MAC7)
	$(MD) $(dir $@)
	$(CC) $(CFLAGS_MAC7) -c -o $@ $(<:%.o=%)

# ====== Check: NDS ======

$(OBJDIR_NDS):
	$(MD) $@

$(GENDIR_NDS):
	$(MD) $@

$(GENDIR_NDS)/%.h: $(ASSETDIR)/%.json $(MAPC) | $(GENDIR_NDS)
	$(PYTHON) $(MAPC) -j $< -o $@

$(GENDIR_NDS)/resext.h: \
$(HEADPACK) $(DSEKAI_ASSETS_BITMAPS) $(DSEKAI_ASSETS_MAPS) | $(GENDIR_NDS)
	$(HEADPACK) $@ $(DSEKAI_ASSETS_BITMAPS) $(DSEKAI_ASSETS_MAPS)

$(BIN_NDS): $(OBJDIR_NDS)/dsekai.elf $(GENDIR_NDS)/dsekai-1.bmp
	ndstool -c $@ -9 $< -b $(GENDIR_NDS)/dsekai-1.bmp "dsekai;dsekai;dsekai"

$(GENDIR_NDS)/dsekai-1.bmp: $(ASSETDIR)/dsekai.ico
	$(IMAGEMAGICK) $< -compress none -colors 16 $(GENDIR_NDS)/dsekai.bmp

$(OBJDIR_NDS)/dsekai.elf: $(DSEKAI_O_FILES_NDS)
	$(CC) $(LDFLAGS) $^ $(LIBS) -o $@

$(OBJDIR_NDS)/%.o: \
%.c $(DSEKAI_ASSETS_MAPS_NDS) $(GENDIR_NDS)/resext.h | $(OBJDIR_NDS)
	$(MD) $(dir $@)
	$(CC) $(CFLAGS_NDS) -c -o $@ $(<:%.o=%)

$(DEPDIR_NDS)/%.d: %.c $(GENDIR_NDS)/resext.h
	$(MD) $(dir $@)
	arm-none-eabi-gcc $(CFLAGS_NDS) -MM $< \
      -MT $(subst .c,.o,$(addprefix $(DEPDIR_NDS)/,$<)) -MF $@ || touch $@

include $(subst obj/,dep/,$(DSEKAI_O_FILES_NDS:.o=.d))

# ====== Check: Null ======

$(GENDIR_CHECK_NULL):
	$(MD) $@

$(GENDIR_CHECK_NULL)/resext.h: $(GENDIR_CHECK_NULL) $(MKRESH)
	$(MKRESH) -f palm -i 5001 \
      -if $(DSEKAI_ASSETS_PALM) $(DSEKAI_ASSETS_MAPS) \
      -oh $(GENDIR_CHECK_NULL)/resext.h

$(BIN_CHECK_NULL): $(DSEKAI_O_FILES_CHECK_NULL) | $(BINDIR)
	$(CC) -o $@ $^ $(LDFLAGS)

$(DEPDIR_CHECK_NULL)/%.d: %.c $(GENDIR_CHECK_NULL)/resext.h
	$(MD) $(dir $@)
	$(CC) $(CFLAGS_CHECK_NULL) -MM $< \
      -MT $(subst .c,.o,$(addprefix $(DEPDIR_CHECK_NULL)/,$<)) -MF $@

include $(subst obj/,dep/,$(DSEKAI_O_FILES_CHECK_NULL:.o=.d))
	
$(OBJDIR_CHECK_NULL)/%.o: %.c check/testdata.h $(GENDIR_CHECK_NULL)/resext.h
	$(MD) $(dir $@)
	$(CC) $(CFLAGS_CHECK_NULL) -c -o $@ $(<:%.o=%)

# ====== Clean ======

clean:
	rm -rf data obj bin gen *.err .rsrc .finf

