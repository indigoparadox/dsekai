
DSEKAI := "dsekai"

DSEKAI_C_FILES := \
   src/tilemap.c \
   src/graphics.c \
   src/mobile.c \
   src/item.c \
   src/window.c \
   src/dio.c \
   src/control.c \
   src/topdown.c

DSEKAI_C_FILES_WIN_ONLY := \
   src/main.c \
   src/input/wini.c \
   src/resource/winr.c \
   src/memory/winm.c \
   src/graphics/wing.c

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
   tools/data/header.c \
   tools/data/icns.c \
   src/json.c

LOOKUPS_C_FILES := \
   tools/lookups.c

MAP2H_C_FILES := \
   tools/map2h.c \
   src/tilemap.c \
   src/mobile.c \
   src/memory/fakem.c \
   src/dio.c \
   src/json.c \
   src/resource/nullr.c \
   src/graphics.c \
   src/graphics/nullg.c

ASSETDIR := assets
OBJDIR := obj
DEPDIR := dep
GENDIR := gen

OBJDIR_WIN16 :=      $(OBJDIR)/win16
OBJDIR_WIN32 :=      $(OBJDIR)/win32
OBJDIR_CHECK_NULL := $(OBJDIR)/check_null

DEPDIR_WIN16 :=      $(DEPDIR)/win16
DEPDIR_WIN32 :=      $(DEPDIR)/win32
DEPDIR_CHECK_NULL := $(DEPDIR)/check_null

GENDIR_WIN16 := $(GENDIR)/win16
GENDIR_WIN32 := $(GENDIR)/win32
GENDIR_CHECK_NULL := $(GENDIR)/check_null

BINDIR := bin

BIN_SDL := $(BINDIR)/dsekai
BIN_DOS := $(BINDIR)/dsekai.exe
BIN_XLIB := $(BINDIR)/dsekaix
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
DSEKAI_ASSETS_MAPS_WIN16 := \
   $(subst .json,.h,$(subst $(ASSETDIR)/,$(GENDIR_WIN16)/,$(DSEKAI_ASSETS_MAPS)))
DSEKAI_ASSETS_MAPS_WIN32 := \
   $(subst .json,.h,$(subst $(ASSETDIR)/,$(GENDIR_WIN32)/,$(DSEKAI_ASSETS_MAPS)))

HOST_CC := gcc
MD := mkdir -p
DD := /bin/dd
MCOPY := mcopy
MKFSVFAT := /sbin/mkfs.vfat
IMAGEMAGICK := convert
PYTHON := python3

MKRESH := bin/mkresh
DRCPACK := bin/drcpack
CONVERT := bin/convert
LOOKUPS := bin/lookups
HEADPACK := bin/headpack
MAP2H := bin/map2h

CFLAGS_MKRESH := -DNO_RESEXT -g -DDEBUG_LOG -DDEBUG_THRESHOLD=0 -DUSE_JSON_MAPS -DRESOURCE_DRC
CFLAGS_DRCPACK := -DNO_RESEXT -g -DDRC_READ_WRITE -DDEBUG_LOG -DDEBUG_THRESHOLD=3 -DUSE_JSON_MAPS -DRESOURCE_DRC
CFLAGS_CONVERT := -DNO_RESEXT -g -DUSE_JSON_MAPS -DRESOURCE_DRC
CFLAGS_LOOKUPS := -g
CFLAGS_HEADPACK := -g
CFLAGS_MAP2H := -g

CFLAGS_DEBUG_GENERIC := -DDEBUG_LOG -DDEBUG_THRESHOLD=2
CFLAGS_DEBUG_GCC := $(CFLAGS_DEBUG_GENERIC) -Wall -Wno-missing-braces -Wno-char-subscripts -fsanitize=address -fsanitize=leak -fsanitize=undefined -pg

CFLAGS_WIN16 := -bt=windows -i=$(INCLUDE)/win -bw -DSCREEN_SCALE=2 -DPLATFORM_WIN16 $(CFLAGS_DEBUG_GENERIC) -zp=1
CFLAGS_WIN32 := -bt=nt -3 -i=$(INCLUDE) -i=$(INCLUDE)/nt -DSCREEN_SCALE=2 -DPLATFORM_WIN32 $(CFLAGS_DEBUG_GENERIC) -zp=1
CFLAGS_CHECK_NULL := -DSCREEN_SCALE=3 $(shell pkg-config check --cflags) -g -DSCREEN_W=160 -DSCREEN_H=160 -std=c89 -DPLATFORM_NULL $(CFLAGS_DEBUG_GCC) -DRESOURCE_DRC

$(BIN_WIN16): CC := wcc
$(BIN_WIN16): LD := wcl
$(BIN_WIN16): RC := wrc
$(BIN_WIN16): LDFLAGS := -l=windows -zp=1

$(BIN_WIN32): CC := wcc386
$(BIN_WIN32): LD := wcl386
$(BIN_WIN32): RC := wrc
$(BIN_WIN32): LDFLAGS := -bcl=nt_win -zp=1

$(BIN_CHECK_NULL): LDFLAGS := $(shell pkg-config check --libs) -g $(CFLAGS_DEBUG_GCC)

DSEKAI_C_FILES_CHECK_NULL := $(DSEKAI_C_FILES) $(DSEKAI_C_FILES_CHECK_NULL_ONLY)

DSEKAI_O_FILES_WIN16 := \
   $(addprefix $(OBJDIR_WIN16)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_WIN16)/,$(subst .c,.o,$(DSEKAI_C_FILES_WIN_ONLY)))
DSEKAI_O_FILES_WIN32 := \
   $(addprefix $(OBJDIR_WIN32)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_WIN32)/,$(subst .c,.o,$(DSEKAI_C_FILES_WIN_ONLY)))
DSEKAI_O_FILES_CHECK_NULL := \
   $(addprefix $(OBJDIR_CHECK_NULL)/,$(subst .c,.o,$(DSEKAI_C_FILES_CHECK_NULL)))

.PHONY: clean grc_palm

all: $(BIN_DOS) $(BIN_SDL) $(BIN_PALM)

STAMPFILE := .stamp

$(BINDIR)/$(STAMPFILE):
	$(MD) $(BINDIR)
	touch $@

$(OBJDIR)/%/$(STAMPFILE):
	$(MD) $(dir $@)
	touch $@

$(GENDIR)/%/$(STAMPFILE):
	$(MD) $(dir $@)
	touch $@

$(GENDIR)/%/resext.h: \
$(DSEKAI_ASSETS_BITMAPS) $(DSEKAI_ASSETS_MAPS) | \
$(GENDIR)/%/$(STAMPFILE) $(HEADPACK)
	$(HEADPACK) $@ $^

# ====== Utilities ======

$(MKRESH): $(MKRESH_C_FILES) | $(BINDIR)/$(STAMPFILE)
	$(HOST_CC) $(CFLAGS_MKRESH) -o $@ $^

$(DRCPACK): $(DRCPACK_C_FILES) | $(BINDIR)/$(STAMPFILE)
	$(HOST_CC) $(CFLAGS_DRCPACK) -o $@ $^

$(CONVERT): $(CONVERT_C_FILES) | $(BINDIR)/$(STAMPFILE)
	$(HOST_CC) $(CFLAGS_CONVERT) -o $@ $^

$(LOOKUPS): $(LOOKUPS_C_FILES) | $(BINDIR)/$(STAMPFILE)
	$(HOST_CC) $(CFLAGS_LOOKUPS) -o $@ $^

$(HEADPACK): tools/headpack.c | $(BINDIR)/$(STAMPFILE)
	$(HOST_CC) $(CFLAGS_HEADPACK) -o $@ tools/headpack.c

$(MAP2H): $(MAP2H_C_FILES) | $(BINDIR)/$(STAMPFILE)
	$(HOST_CC) $(CFLAGS_MAP2H) -o $@ $^

# ====== Main: SDL ======

# 1. Directories

OBJDIR_SDL := $(OBJDIR)/sdl
DEPDIR_SDL := $(DEPDIR)/sdl
GENDIR_SDL := $(GENDIR)/sdl

# 2. Files

DSEKAI_C_FILES_SDL_ONLY := \
   src/main.c \
   src/input/sdli.c \
   src/graphics/sdlg.c \
   src/memory/fakem.c \
   src/resource/header.c

DSEKAI_O_FILES_SDL := \
   $(addprefix $(OBJDIR_SDL)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_SDL)/,$(subst .c,.o,$(DSEKAI_C_FILES_SDL_ONLY)))

# 3. Programs

CC_SDL := gcc

# 4. Arguments

CFLAGS_SDL := -DSCREEN_SCALE=3 $(shell pkg-config sdl2 --cflags) -g -DSCREEN_W=160 -DSCREEN_H=160 -std=c89 -DPLATFORM_SDL $(CFLAGS_DEBUG_GCC)

LDFLAGS_SDL := $(shell pkg-config sdl2 --libs) -g $(CFLAGS_DEBUG_GCC)

# 5. Targets

$(BIN_SDL): $(DSEKAI_O_FILES_SDL) src/json.o | $(BINDIR)/$(STAMPFILE)
	$(CC_SDL) -o $@ $^ $(LDFLAGS_SDL)

$(OBJDIR_SDL)/%.o: %.c $(GENDIR_SDL)/resext.h | $(DSEKAI_ASSETS_MAPS)
	$(MD) $(dir $@)
	$(CC_SDL) $(CFLAGS_SDL) -DUSE_JSON_MAPS -c -o $@ $(<:%.o=%)

$(DEPDIR_SDL)/%.d: %.c $(GENDIR_SDL)/resext.h
	$(MD) $(dir $@)
	$(CC_SDL) $(CFLAGS_SDL) -DUSE_JSON_MAPS -MM $< \
      -MT $(subst .c,.o,$(addprefix $(DEPDIR_SDL)/,$<)) -MF $@

include $(subst $(OBJDIR)/,$(DEPDIR)/,$(DSEKAI_O_FILES_SDL:.o=.d))

# ====== Main: SDL (No JSON) ======

# 1. Directories

# 2. Files

DSEKAI_ASSETS_MAPS_SDL_NJ := \
$(subst .json,.h,$(subst $(ASSETDIR)/,$(GENDIR_SDL)-nj/,$(DSEKAI_ASSETS_MAPS)))

# 3. Programs

# 4. Arguments

LDFLAGS_SDL_NJ := $(shell pkg-config sdl2 --libs) -g $(CFLAGS_DEBUG_GCC)

# 5. Targets

$(GENDIR_SDL)-nj/map_%.h: $(ASSETDIR)/map_%.json $(MAP2H) | \
$(GENDIR_SDL)-nj/$(STAMPFILE)
	$(MAP2H) $< $@

$(BIN_SDL)-nj: $(subst /sdl/,/sdl-nj/,$(DSEKAI_O_FILES_SDL)) | \
$(BINDIR) $(DSEKAI_ASSETS_MAPS_SDL_NJ)
	$(CC_SDL) -o $@ $^ $(LDFLAGS_SDL_NJ)

$(OBJDIR_SDL)-nj/%.o: %.c $(GENDIR_SDL)-nj/resext.h | \
$(DSEKAI_ASSETS_MAPS_SDL_NJ)
	$(MD) $(dir $@)
	$(CC_SDL) $(CFLAGS_SDL) -c -o $@ $(<:%.o=%)

# ====== Main: xlib ======

# 1. Directories

OBJDIR_XLIB := $(OBJDIR)/xlib
DEPDIR_XLIB := $(DEPDIR)/xlib
GENDIR_XLIB := $(GENDIR)/xlib

# 2. Files

DSEKAI_C_FILES_XLIB_ONLY := \
   src/main.c \
   src/input/xi.c \
   src/graphics/xg.c \
   src/memory/fakem.c \
   src/resource/header.c

DSEKAI_ASSETS_MAPS_XLIB := \
   $(subst .json,.h,$(subst $(ASSETDIR)/,$(GENDIR_XLIB)/,$(DSEKAI_ASSETS_MAPS)))

# 3. Programs

CC_XLIB := gcc
LD_XLIB := gcc

# 4. Arguments

CFLAGS_XLIB := -DSCREEN_SCALE=3 -g -DSCREEN_W=160 -DSCREEN_H=160 -std=c89 -DPLATFORM_XLIB $(CFLAGS_DEBUG_GCC)

LDFLAGS_XLIB := -g -lX11 $(CFLAGS_DEBUG_GCC)

DSEKAI_O_FILES_XLIB := \
   $(addprefix $(OBJDIR_XLIB)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_XLIB)/,$(subst .c,.o,$(DSEKAI_C_FILES_XLIB_ONLY)))

# 5. Targets

$(GENDIR_XLIB)/map_%.h: $(ASSETDIR)/map_%.json | \
$(GENDIR_XLIB)/$(STAMPFILE) $(MAP2H)
	$(MAP2H) $< $@

$(BIN_XLIB): $(DSEKAI_O_FILES_XLIB) | $(BINDIR) $(DSEKAI_ASSETS_MAPS_XLIB)
	$(LD_XLIB) -o $@ $^ $(LDFLAGS_XLIB)

$(OBJDIR_XLIB)/%.o: %.c $(GENDIR_XLIB)/resext.h | $(DSEKAI_ASSETS_MAPS_XLIB)
	$(MD) $(dir $@)
	$(CC_XLIB) $(CFLAGS_XLIB) -c -o $@ $(<:%.o=%)

$(DEPDIR_XLIB)/%.d: %.c $(GENDIR_XLIB)/resext.h | $(DSEKAI_ASSETS_MAPS_XLIB)
	$(MD) $(dir $@)
	$(CC_XLIB) $(CFLAGS_XLIB) -MM $< \
      -MT $(subst .c,.o,$(addprefix $(DEPDIR_XLIB)/,$<)) -MF $@

include $(subst $(OBJDIR)/,$(DEPDIR)/,$(DSEKAI_O_FILES_XLIB:.o=.d))

# ====== Main: MS-DOS ======

# 1. Directories

OBJDIR_DOS := $(OBJDIR)/dos
DEPDIR_DOS := $(DEPDIR)/dos
GENDIR_DOS := $(GENDIR)/dos

# 2. Files

DSEKAI_C_FILES_DOS_ONLY := \
   src/main.c \
   src/input/dosi.c \
   src/graphics/dosg.c \
   src/memory/fakem.c \
   src/resource/header.c

DSEKAI_ASSETS_MAPS_DOS := \
   $(subst .json,.h,$(subst $(ASSETDIR)/,$(GENDIR_DOS)/,$(DSEKAI_ASSETS_MAPS)))

DSEKAI_ASSETS_DOS_CGA := \
   $(subst .bmp,.cga,$(subst $(ASSETDIR)/,$(GENDIR_DOS)/,$(DSEKAI_ASSETS_BITMAPS)))

DSEKAI_O_FILES_DOS := \
   $(addprefix $(OBJDIR_DOS)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_DOS)/,$(subst .c,.o,$(DSEKAI_C_FILES_DOS_ONLY)))

# 3. Programs

CC_DOS := wcc
LD_DOS := wcl

# 4. Arguments

CFLAGS_DOS := -hw -d3 -0 -ms -DPLATFORM_DOS -DUSE_LOOKUPS -zp=1

LDFLAGS_DOS := $(CFLAGS_DOS)

# 5. Targets

$(GENDIR_DOS)/resext.h: \
$(DSEKAI_ASSETS_DOS_CGA) $(DSEKAI_ASSETS_MAPS) | $(HEADPACK)
	$(HEADPACK) $@ $^

#$(BINDIR)/doscga.drc: res_doscga_drc

#res_doscga_drc: $(DRCPACK) $(DSEKAI_ASSETS_DOS_CGA)
#	rm $(BINDIR)/doscga.drc || true
#	$(DRCPACK) -c -a -af $(BINDIR)/doscga.drc -i 5001 \
#      -if $(GENDIR_DOS)/*.cga $(DSEKAI_ASSETS_MAPS) -lh $(GENDIR_DOS)/resext.h

$(GENDIR_DOS)/map_%.h: $(ASSETDIR)/map_%.json $(MAP2H) | $(GENDIR_DOS)/$(STAMPFILE)
	$(MAP2H) $< $@

$(GENDIR_DOS)/%.cga: $(ASSETDIR)/%.bmp $(CONVERT) | $(GENDIR_DOS)/$(STAMPFILE)
	$(CONVERT) -ic bitmap -oc cga -ob 2 -if $< -of $@ -og

$(BIN_DOS): $(DSEKAI_O_FILES_DOS) | $(BINDIR) $(DSEKAI_ASSETS_MAPS_DOS)
	$(LD_DOS) $(LDFLAGS_DOS) -fe=$@ $^

$(OBJDIR_DOS)/%.o: %.c $(GENDIR_DOS)/resext.h $(DSEKAI_ASSETS_MAPS_DOS)
	$(MD) $(dir $@)
	$(CC_DOS) $(CFLAGS_DOS) -fo=$@ $(<:%.c=%)

# ====== Main: Palm ======

# 1. Directories

OBJDIR_PALM := $(OBJDIR)/palm
DEPDIR_PALM := $(DEPDIR)/palm
GENDIR_PALM := $(GENDIR)/palm

# 2. Files

DSEKAI_C_FILES_PALM_ONLY := \
   src/main.c \
   src/json.c \
   src/input/palmi.c \
   src/memory/palmm.c \
   src/resource/palmr.c \
   src/graphics/palmg.c

DSEKAI_ASSETS_PALM := \
   $(subst $(ASSETDIR)/,$(GENDIR_PALM)/,$(DSEKAI_ASSETS_BITMAPS))

DSEKAI_O_FILES_PALM := \
   $(addprefix $(OBJDIR_PALM)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_PALM)/,$(subst .c,.o,$(DSEKAI_C_FILES_PALM_ONLY)))

# 3. Programs

CC_PALM := m68k-palmos-gcc
LD_PALM := m68k-palmos-gcc
PILRC := pilrc
OBJRES := m68k-palmos-obj-res
BUILDPRC := build-prc

# 4. Arguments

CFLAGS_PALM := -Os -DSCREEN_W=160 -DSCREEN_H=160 -I /opt/palmdev/sdk-3.5/include -I /opt/palmdev/sdk-3.5/include/Core/UI/ -I /opt/palmdev/sdk-3.5/include/Core/System/ -I /opt/palmdev/sdk-3.5/include/Core/Hardware/ -I /opt/palmdev/sdk-3.5/include/Core/International/ -DPLATFORM_PALM -g $(CFLAGS_DEBUG_GENERIC)

LDFLAGS_PALM = -g $(CFLAGS_PALM)

APPID := DSEK

# 5. Targets

$(GENDIR_PALM)/%.bmp: $(ASSETDIR)/%.bmp $(CONVERT) | $(GENDIR_PALM)/$(STAMPFILE)
	$(CONVERT) -if $< -of $@ -ob 1 -r -ic bitmap -oc bitmap

$(GENDIR_PALM)/resext.h \
$(GENDIR_PALM)/palmd.rcp: \
$(DSEKAI_ASSETS_PALM) $(DSEKAI_ASSETS_MAPS) | $(GENDIR_PALM)/$(STAMPFILE) $(MKRESH)
	$(MKRESH) -f palm -i 5001 \
      -if $(DSEKAI_ASSETS_PALM) $(DSEKAI_ASSETS_MAPS) \
      -oh $(GENDIR_PALM)/resext.h \
      -or $(GENDIR_PALM)/palmd.rcp

grc_palm: $(OBJDIR_PALM)/dsekai
	cd $(OBJDIR_PALM) && $(OBJRES) dsekai

$(OBJDIR_PALM)/dsekai: $(DSEKAI_O_FILES_PALM)
	$(LD_PALM) $(LDFLAGS_PALM) $^ -o $@
	
$(OBJDIR_PALM)/bin$(STAMPFILE): src/palms.rcp $(GENDIR_PALM)/palmd.rcp
	$(PILRC) $< $(OBJDIR_PALM)
	touch $@

$(BIN_PALM): grc_palm $(OBJDIR_PALM)/bin$(STAMPFILE) | $(BINDIR)/$(STAMPFILE)
	$(BUILDPRC) $@ $(DSEKAI) $(APPID) $(OBJDIR_PALM)/*.grc $(OBJDIR_PALM)/*.bin

$(OBJDIR_PALM)/%.o: %.c $(GENDIR_PALM)/palmd.rcp $(GENDIR_PALM)/resext.h
	$(MD) $(dir $@)
	$(CC_PALM) $(CFLAGS_PALM) -c -o $@ $(<:%.o=%)

# ====== Main: Win16 ======

$(GENDIR_WIN16):
	$(MD) $@

$(GENDIR_WIN16)/%.ico: $(ASSETDIR)/%.bmp | $(GENDIR_WIN16)/$(STAMPFILE)
	$(IMAGEMAGICK) $< $@

$(GENDIR_WIN16)/%.h: $(ASSETDIR)/%.json $(MAP2H) | $(GENDIR_WIN16)/$(STAMPFILE)
	$(MAP2H) $< $@

$(BINDIR)/dsekai16.img: $(BIN_WIN16)
	$(DD) if=/dev/zero bs=512 count=2880 of="$@"
	$(MKFSVFAT) "$@"
	$(MCOPY) -i "$@" $< ::dsekai16.exe

$(BIN_WIN16): \
$(DSEKAI_O_FILES_WIN16) $(OBJDIR_WIN16)/win16.res | \
$(BINDIR)/$(STAMPFILE) $(DSEKAI_ASSETS_MAPS_WIN16)
	$(LD) $(LDFLAGS) -fe=$@ $^

$(OBJDIR_WIN16)/win16.res: \
$(GENDIR_WIN16)/win16.rc $(ASSETDIR)/dsekai.ico | $(OBJDIR_WIN16)/$(STAMPFILE)
	$(RC) -r -DPLATFORM_WIN16 -i $(INCLUDE)win src/winstat.rc -o $@

$(GENDIR_WIN16)/win16.rc \
$(GENDIR_WIN16)/resext.h: $(DSEKAI_ASSETS_BITMAPS) $(MKRESH) | \
$(GENDIR_WIN16)/$(STAMPFILE)
	$(MKRESH) -f win16 -i 5001 \
      -if $(DSEKAI_ASSETS_BITMAPS) $(DSEKAI_ASSETS_MAPS) \
      -oh $(GENDIR_WIN16)/resext.h -or $(GENDIR_WIN16)/win16.rc

$(OBJDIR_WIN16)/%.o: \
%.c $(OBJDIR_WIN16)/win16.res $(GENDIR_WIN16)/resext.h $(DSEKAI_ASSETS_MAPS_WIN16)
	$(MD) $(dir $@)
	$(CC) $(CFLAGS_WIN16) -fo=$@ $(<:%.c=%)

$(DEPDIR_WIN16)/%.d: %.c $(GENDIR_WIN16)/resext.h $(DSEKAI_ASSETS_MAPS_WIN16)
	$(MD) $(dir $@)
	$(HOST_CC) -DPLATFORM_WIN16 -MM $< \
      -MT $(subst .c,.o,$(addprefix $(DEPDIR_WIN16)/,$<)) -MF $@ || touch $@

include $(subst $(OBJDIR)/,$(DEPDIR)/,$(DSEKAI_O_FILES_WIN16:.o=.d))

# ====== Main: Win32 ======

$(GENDIR_WIN32)/map_%.h: $(ASSETDIR)/map_%.json $(MAP2H) | \
$(GENDIR_WIN32)/$(STAMPFILE)
	$(MAP2H) $< $@

$(BIN_WIN32): \
$(DSEKAI_O_FILES_WIN32) $(OBJDIR_WIN32)/win32.res | \
$(BINDIR)/$(STAMPFILE) $(DSEKAI_ASSETS_MAPS_WIN32)
	$(LD) $(LDFLAGS) -fe=$@ $^

$(OBJDIR_WIN32)/win32.res: \
$(GENDIR_WIN32)/win32.rc $(ASSETDIR)/dsekai.ico | $(OBJDIR_WIN32)/$(STAMPFILE)
	$(RC) -r -DPLATFORM_WIN32 -i $(INCLUDE)win src/winstat.rc -o $@

$(GENDIR_WIN32)/win32.rc \
$(GENDIR_WIN32)/resext.h: $(DSEKAI_ASSETS_BITMAPS) $(MKRESH) | \
$(GENDIR_WIN32)/$(STAMPFILE)
	$(MKRESH) -f win16 -i 5001 \
      -if $(DSEKAI_ASSETS_BITMAPS) $(DSEKAI_ASSETS_MAPS) \
      -oh $(GENDIR_WIN32)/resext.h -or $(GENDIR_WIN32)/win32.rc

$(OBJDIR_WIN32)/%.o: \
%.c $(OBJDIR_WIN32)/win32.res $(GENDIR_WIN32)/resext.h $(DSEKAI_ASSETS_MAPS_WIN32)
	$(MD) $(dir $@)
	$(CC) $(CFLAGS_WIN32) -fo=$@ $(<:%.c=%)

$(DEPDIR_WIN32)/%.d: %.c $(GENDIR_WIN32)/resext.h $(DSEKAI_ASSETS_MAPS_WIN32)
	$(MD) $(dir $@)
	$(HOST_CC) -DPLATFORM_WIN32 -MM $< \
      -MT $(subst .c,.o,$(addprefix $(DEPDIR_WIN32)/,$<)) -MF $@ || touch $@

include $(subst $(OBJDIR)/,$(DEPDIR)/,$(DSEKAI_O_FILES_WIN32:.o=.d))

# ====== Main: MacOS 7 ======

# 1. Directories

OBJDIR_MAC7 := $(OBJDIR)/mac7
DEPDIR_MAC7 := $(DEPDIR)/mac7
GENDIR_MAC7 := $(GENDIR)/mac7

RETRO68_PREFIX := /opt/Retro68-build/toolchain

# 2. Files

DSEKAI_C_FILES_MAC7_ONLY := \
   src/main.c \
   src/input/mac7i.c \
   src/graphics/mac7g.c \
   src/memory/mac7m.c \
   src/resource/header.c

DSEKAI_ASSETS_ICNS := \
   $(subst .bmp,.icns,$(subst $(ASSETDIR)/,$(GENDIR_MAC7)/,$(DSEKAI_ASSETS_BITMAPS)))

DSEKAI_ASSETS_RSRC := \
   $(subst .bmp,.rsrc,$(subst $(ASSETDIR)/,$(GENDIR_MAC7)/,$(DSEKAI_ASSETS_BITMAPS)))

DSEKAI_ASSETS_MAPS_MAC7 := \
   $(subst .json,.h,$(subst $(ASSETDIR)/,$(GENDIR_MAC7)/,$(DSEKAI_ASSETS_MAPS)))

DSEKAI_O_FILES_MAC7 := \
   $(addprefix $(OBJDIR_MAC7)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_MAC7)/,$(subst .c,.o,$(DSEKAI_C_FILES_MAC7_ONLY)))

DSEKAI_ASSETS_PICTS := \
   $(subst .bmp,.pict,$(subst $(ASSETDIR)/,$(GENDIR_MAC7)/,$(DSEKAI_ASSETS_BITMAPS)))

# 3. Programs

CC_MAC7 := m68k-apple-macos-gcc
CXX_MAC7 := m68k-apple-macos-g++
LDFLAGS_MAC7 := -lRetroConsole
REZ_MAC7 := Rez

# 4. Arguments

CFLAGS_MAC7 := -DPLATFORM_MAC7 -I$(RETRO68_PREFIX)/multiversal/CIncludes $(CFLAGS_DEBUG_GENERIC)

# 5. Targets

$(GENDIR_MAC7)/%.h: $(ASSETDIR)/%.json $(MAP2H) | $(GENDIR_MAC7)/$(STAMPFILE)
	$(MAP2H) $< $@

#$(BINDIR)/mac7.drc \
#$(GENDIR_MAC7)/resext.h: $(DSEKAI_ASSETS_PICTS) $(DRCPACK) | $(GENDIR_SDL)
#	$(DRCPACK) -c -a -af $(BINDIR)/mac7.drc -t PICT -i 5001 \
#      -if $(DSEKAI_ASSETS_PICTS) $(DSEKAI_ASSETS_MAPS) \
#      -lh $(GENDIR_MAC7)/resext.h

$(GENDIR_MAC7)/resext.h: \
$(DSEKAI_ASSETS_PICTS) $(DSEKAI_ASSETS_MAPS) | $(GENDIR_MAC7) $(HEADPACK)
	$(HEADPACK) $@ $^

$(GENDIR_MAC7)/%.pict: $(ASSETDIR)/%.bmp | $(GENDIR_MAC7)/$(STAMPFILE)
	$(IMAGEMAGICK) $< $@

#$(GENDIR_MAC7)/%.rsrc: $(GENDIR_MAC7)/%.icns
#	echo "read 'icns' (-16455) \"$<\";" > $@

$(GENDIR_MAC7)/%.icns: $(ASSETDIR)/%.bmp $(CONVERT) | $(GENDIR_MAC7)/$(STAMPFILE)
	$(CONVERT) -if $< -of $@ -ob 1 -r -ic bitmap -oc icns

$(OBJDIR_MAC7)/dsekai.code.bin: $(DSEKAI_O_FILES_MAC7) | $(OBJDIR_MAC7)
	$(CXX_MAC7) $(LDFLAGS_MAC7) -o $@ $^ # Use CXX to link for RetroConsole.

$(BIN_MAC7): $(OBJDIR_MAC7)/dsekai.code.bin
	$(REZ_MAC7) -I$(RETRO68_PREFIX)/RIncludes \
      --copy $^ \
      "$(RETRO68_PREFIX)/RIncludes/Retro68APPL.r" \
      -t "APPL" -c "DSEK" \
      -o $(BINDIR)/dsekai.bin \
      --cc $(BINDIR)/dsekai.APPL \
      --cc $(BINDIR)/dsekai.dsk

$(OBJDIR_MAC7)/%.o: \
%.c $(DSEKAI_ASSETS_MAPS_MAC7) $(GENDIR_MAC7)/resext.h
	$(MD) $(dir $@)
	$(CC_MAC7) $(CFLAGS_MAC7) -c -o $@ $(<:%.o=%)

# ====== Check: NDS ======

# 1. Directories

OBJDIR_NDS := $(OBJDIR)/nds
DEPDIR_NDS := $(DEPDIR)/nds
GENDIR_NDS := $(GENDIR)/nds

DEVKITPATH := $(shell echo "$(DEVKITPRO)" | sed -e 's/^\([a-zA-Z]\):/\/\1/')

# 2. Files

DSEKAI_C_FILES_NDS_ONLY := \
   src/main.c \
   src/input/ndsi.c \
   src/graphics/ndsg.c \
   src/memory/fakem.c \
   src/resource/header.c

DSEKAI_ASSETS_MAPS_NDS := \
   $(subst .json,.h,$(subst $(ASSETDIR)/,$(GENDIR_NDS)/,$(DSEKAI_ASSETS_MAPS)))

DSEKAI_O_FILES_NDS := \
   $(addprefix $(OBJDIR_NDS)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_NDS)/,$(subst .c,.o,$(DSEKAI_C_FILES_NDS_ONLY)))

# 3. Programs

CC_NDS := arm-none-eabi-gcc
LD_NDS := arm-none-eabi-gcc
NDSTOOL := ndstool

# 4. Arguments

ARCH_NDS := -mthumb -mthumb-interwork

CFLAGS_NDS := --sysroot $(DEVKITARM)/arm-none-eabi -I$(DEVKITPRO)/libnds/include -DPLATFORM_NDS -DARM9 -g -march=armv5te -mtune=arm946e-s -fomit-frame-pointer -ffast-math $(ARCH_NDS)

LIBS_NDS := -L$(DEVKITPRO)/libnds/lib -lnds9

LDFLAGS_NDS := -specs=ds_arm9.specs -g $(ARCH_NDS) -Wl,-Map,$(OBJDIR_NDS)/dsekai.map

$(BIN_NDS): PATH := $(DEVKITPATH)/tools/bin:$(DEVKITPATH)/devkitARM/bin:$(PATH)

# 5. Targets

$(GENDIR_NDS)/%.h: $(ASSETDIR)/%.json $(MAP2H) | $(GENDIR_NDS)/$(STAMPFILE)
	$(MAP2H) $< $@

$(BIN_NDS): $(OBJDIR_NDS)/dsekai.elf $(GENDIR_NDS)/dsekai-1.bmp
	$(NDSTOOL) -c $@ -9 $< -b $(GENDIR_NDS)/dsekai-1.bmp "dsekai;dsekai;dsekai"

$(GENDIR_NDS)/dsekai-1.bmp: $(ASSETDIR)/dsekai.ico
	$(IMAGEMAGICK) $< -compress none -colors 16 $(GENDIR_NDS)/dsekai.bmp

$(OBJDIR_NDS)/dsekai.elf: $(DSEKAI_O_FILES_NDS)
	$(LD_NDS) $(LDFLAGS_NDS) $^ $(LIBS_NDS) -o $@

$(OBJDIR_NDS)/%.o: \
%.c $(DSEKAI_ASSETS_MAPS_NDS) $(GENDIR_NDS)/resext.h
	$(MD) $(dir $@)
	$(CC_NDS) $(CFLAGS_NDS) -c -o $@ $(<:%.o=%)

$(DEPDIR_NDS)/%.d: %.c $(GENDIR_NDS)/resext.h $(DSEKAI_ASSETS_MAPS_NDS)
	$(MD) $(dir $@)
	$(CC_NDS) $(CFLAGS_NDS) -MM $< \
      -MT $(subst .c,.o,$(addprefix $(DEPDIR_NDS)/,$<)) -MF $@ || touch $@

include $(subst $(OBJDIR)/,$(DEPDIR)/,$(DSEKAI_O_FILES_NDS:.o=.d))

# ====== Check: Null ======

$(GENDIR_CHECK_NULL)/resext.h: $(GENDIR_CHECK_NULL)/$(STAMPFILE) $(MKRESH)
	$(MKRESH) -f palm -i 5001 \
      -if $(DSEKAI_ASSETS_PALM) $(DSEKAI_ASSETS_MAPS) \
      -oh $(GENDIR_CHECK_NULL)/resext.h

$(BIN_CHECK_NULL): $(DSEKAI_O_FILES_CHECK_NULL) | $(BINDIR)/$(STAMPFILE)
	$(CC) -o $@ $^ $(LDFLAGS)

$(DEPDIR_CHECK_NULL)/%.d: %.c $(GENDIR_CHECK_NULL)/resext.h
	$(MD) $(dir $@)
	$(CC) $(CFLAGS_CHECK_NULL) -MM $< \
      -MT $(subst .c,.o,$(addprefix $(DEPDIR_CHECK_NULL)/,$<)) -MF $@

include $(subst $(OBJDIR)/,$(DEPDIR)/,$(DSEKAI_O_FILES_CHECK_NULL:.o=.d))
	
$(OBJDIR_CHECK_NULL)/%.o: %.c check/testdata.h $(GENDIR_CHECK_NULL)/resext.h
	$(MD) $(dir $@)
	$(CC) $(CFLAGS_CHECK_NULL) -c -o $@ $(<:%.o=%)

# ====== Clean ======

clean:
	rm -rf data obj bin gen *.err .rsrc .finf

