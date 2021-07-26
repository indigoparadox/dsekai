
DSEKAI := dsekai

DSEKAI_C_FILES := \
   src/tilemap.c \
   unilayer/graphics.c \
   unilayer/resource/header.c \
   unilayer/resource/file.c \
   src/mobile.c \
   src/item.c \
   src/window.c \
   unilayer/dio.c \
   src/control.c \
   src/topdown.c

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
   unilayer/resource/drcr.c \
   check/ckdio.c \
   check/ckdrc.c \
   check/ckmemory.c \
   unilayer/graphics/nullg.c \
   unilayer/input/nulli.c \
   tools/data/cga.c \
   tools/data/bmp.c \
   tools/data/icns.c \
   tools/data/drcwrite.c \
   unilayer/memory/fakem.c \
   unilayer/drc.c \
   unilayer/dio.c

MKRESH_C_FILES := \
   tools/mkresh.c \
   unilayer/memory/fakem.c \
   unilayer/drc.c \
   unilayer/dio.c

DRCPACK_C_FILES := \
   tools/drcpack.c \
   tools/data/drcwrite.c \
   unilayer/memory/fakem.c \
   unilayer/drc.c \
   unilayer/dio.c

CONVERT_C_FILES := \
   tools/convert.c \
   tools/data/bmp.c \
   unilayer/memory/fakem.c \
   unilayer/drc.c \
   tools/data/cga.c \
   unilayer/dio.c \
   tools/data/header.c \
   tools/data/icns.c \
   src/json.c

LOOKUPS_C_FILES := \
   tools/lookups.c

MAP2H_C_FILES := \
   tools/map2h.c \
   src/tilemap.c \
   src/mobile.c \
   unilayer/memory/fakem.c \
   unilayer/dio.c \
   src/json.c \
   unilayer/resource/nullr.c \
   unilayer/graphics.c \
   unilayer/graphics/nullg.c

PLATFORMS := sdl sdl-nj sdl-file xlib dos win16 win32 palm mac6 nds curses check_null

ASSETDIR := assets
OBJDIR := obj
DEPDIR := dep
GENDIR := gen

OBJDIR_CHECK_NULL := $(OBJDIR)/check_null

DEPDIR_CHECK_NULL := $(DEPDIR)/check_null

GENDIR_CHECK_NULL := $(GENDIR)/check_null

BINDIR := bin

BIN_SDL := $(BINDIR)/$(DSEKAI)
BIN_DOS := $(BINDIR)/$(DSEKAI).exe
BIN_XLIB := $(BINDIR)/$(DSEKAI)x
BIN_PALM := $(BINDIR)/$(DSEKAI).prc
BIN_WIN16 := $(BINDIR)/$(DSEKAI)16.exe
BIN_WIN32 := $(BINDIR)/$(DSEKAI)32.exe
BIN_MAC6 := $(BINDIR)/$(DSEKAI).bin $(BINDIR)/$(DSEKAI).APPL $(BINDIR)/$(DSEKAI).dsk
BIN_NDS := $(BINDIR)/$(DSEKAI).nds
BIN_WEB := $(BINDIR)/$(DSEKAI).js
BIN_CURSES := $(BINDIR)/$(DSEKAI)t

BIN_CHECK_NULL := $(BINDIR)/check

CFLAGS_OPT :=
ifeq ($(RESOURCE),"FILE")
CFLAGS_OPT += -DRESOURCE_FILE
else ifeq ($(RESOURCE),"HEADER")
CFLAGS_OPT += -DRESOURCE_HEADER
endif

define BITMAPS_RULE
DSEKAI_ASSETS_SPRITES_$(DEPTH) := \
   $(wildcard $(ASSETDIR)/$(DEPTH)/sprite_*.bmp)
DSEKAI_ASSETS_TILES_$(DEPTH) := \
   $(wildcard $(ASSETDIR)/$(DEPTH)/tile_*.bmp)
DSEKAI_ASSETS_PATTERNS_$(DEPTH) := \
   $(wildcard $(ASSETDIR)/$(DEPTH)/pattern_*.bmp)
DSEKAI_ASSETS_BITMAPS_$(DEPTH) := \
   $(wildcard $(ASSETDIR)/$(DEPTH)/sprite_*.bmp) \
   $(wildcard $(ASSETDIR)/$(DEPTH)/tile_*.bmp) \
   $(wildcard $(ASSETDIR)/$(DEPTH)/pattern_*.bmp)
endef

DEPTHS := 16x16x4

$(foreach DEPTH,$(DEPTHS), $(eval $(BITMAPS_RULE)))

DSEKAI_ASSETS_MAPS := \
   $(ASSETDIR)/map_field.json

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
CFLAGS_MAP2H := -g -DUSE_JSON_MAPS

CFLAGS_DEBUG_GENERIC := -DDEBUG_LOG -DDEBUG_THRESHOLD=3
CFLAGS_DEBUG_GCC := $(CFLAGS_DEBUG_GENERIC) -Wall -Wno-missing-braces -Wno-char-subscripts -fsanitize=address -fsanitize=leak -fsanitize=undefined -pg

CFLAGS_CHECK_NULL := -DSCREEN_SCALE=3 $(shell pkg-config check --cflags) -g -DSCREEN_W=160 -DSCREEN_H=160 -std=c89 -DPLATFORM_NULL $(CFLAGS_DEBUG_GCC) -DRESOURCE_DRC

$(BIN_CHECK_NULL): LDFLAGS := $(shell pkg-config check --libs) -g $(CFLAGS_DEBUG_GCC)

DSEKAI_C_FILES_CHECK_NULL := $(DSEKAI_C_FILES) $(DSEKAI_C_FILES_CHECK_NULL_ONLY)

DSEKAI_O_FILES_CHECK_NULL := \
   $(addprefix $(OBJDIR_CHECK_NULL)/,$(subst .c,.o,$(DSEKAI_C_FILES_CHECK_NULL)))

.PHONY: clean grc_palm

all: $(BIN_DOS) $(BIN_SDL) $(BIN_PALM) $(BIN_XLIB) $(BIN_WIN16) $(BIN_WIN32)

STAMPFILE := .stamp

# ====== Generic Rules ======

$(BINDIR)/$(STAMPFILE):
	$(MD) $(BINDIR)
	touch $@

$(OBJDIR)/%/$(STAMPFILE):
	$(MD) $(dir $@)
	touch $@

$(GENDIR)/%/$(STAMPFILE):
	$(MD) $(dir $@)
	touch $@

ifneq ($(RESOURCE),"FILE")
# This may be overridden by the individual targets, as they may need different
# resources for their platform.
$(GENDIR)/%/resext.h: | $(GENDIR)/%/$(STAMPFILE) $(HEADPACK)
	$(HEADPACK) $@ $^
RESOURCE := HEADER
endif

define ICO_RULE
$(GENDIR)/$(platform)/%.ico: $(ASSETDIR)/%.bmp | \
$(GENDIR)/$(platform)/$(STAMPFILE)
	$(IMAGEMAGICK) $$< $$@
endef

$(foreach platform,$(PLATFORMS), $(eval $(ICO_RULE)))

define MAPS_H_RULE
$(GENDIR)/$(platform)/map_%.h: $(ASSETDIR)/map_%.json | \
$(GENDIR)/$(platform)/$(STAMPFILE) $(MAP2H)
	$(MAP2H) $$< $$@
endef

$(foreach platform,$(PLATFORMS), $(eval $(MAPS_H_RULE)))

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

$(MAP2H): $(MAP2H_C_FILES) | $(BINDIR)/$(STAMPFILE) $(GENDIR)/check_null/resext.h
	$(HOST_CC) $(CFLAGS_MAP2H) -o $@ $^

# ====== Main: SDL ======

# 1. Directories

OBJDIR_SDL := $(OBJDIR)/sdl
DEPDIR_SDL := $(DEPDIR)/sdl
GENDIR_SDL := $(GENDIR)/sdl

# 2. Files

DSEKAI_C_FILES_SDL_ONLY := \
   src/main.c \
   src/json.c \
   unilayer/input/sdli.c \
   unilayer/graphics/sdlg.c \
   unilayer/memory/fakem.c

DSEKAI_O_FILES_SDL := \
   $(addprefix $(OBJDIR_SDL)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_SDL)/,$(subst .c,.o,$(DSEKAI_C_FILES_SDL_ONLY)))

# 3. Programs

CC_SDL := gcc
LD_SDL := gcc

# 4. Arguments

CFLAGS_SDL := -I $(GENDIR_SDL) -DSCREEN_SCALE=3 $(shell pkg-config sdl2 --cflags) -g -DSCREEN_W=160 -DSCREEN_H=160 -std=c89 -DPLATFORM_SDL $(CFLAGS_DEBUG_GCC) -DUSE_SOFTWARE_TEXT $(CFLAGS_OPT)

LDFLAGS_SDL := $(shell pkg-config sdl2 --libs) -g $(CFLAGS_DEBUG_GCC)

# 5. Targets

ifeq ($(RESOURCE),"FILE")
$(GENDIR_SDL)/resext.h: \
$(DSEKAI_ASSETS_BITMAPS_16x16x4) $(DSEKAI_ASSETS_MAPS) | \
$(GENDIR_SDL)/$(STAMPFILE) $(HEADPACK)
else ifeq ($(RESOURCE),"HEADER")
$(GENDIR_SDL)/resext.h: \
$(DSEKAI_ASSETS_BITMAPS_16x16x4) $(DSEKAI_ASSETS_MAPS) | \
$(GENDIR_SDL)/$(STAMPFILE) $(MKRESH)
	$(MKRESH) -f file -if $^ -oh $@
endif

$(BIN_SDL): $(DSEKAI_O_FILES_SDL) | $(BINDIR)/$(STAMPFILE)
	$(LD_SDL) -o $@ $^ $(LDFLAGS_SDL)

$(OBJDIR_SDL)/%.o: %.c $(GENDIR_SDL)/resext.h | $(DSEKAI_ASSETS_MAPS)
	$(MD) $(dir $@)
	$(CC_SDL) $(CFLAGS_SDL) -DUSE_JSON_MAPS -c -o $@ $(<:%.o=%)

$(DEPDIR_SDL)/%.d: %.c $(GENDIR_SDL)/resext.h
	$(MD) $(dir $@)
	$(CC_SDL) $(CFLAGS_SDL) -DUSE_JSON_MAPS -MM $< \
      -MT $(subst .c,.o,$(addprefix $(DEPDIR_SDL)/,$<)) -MF $@

#include $(subst $(OBJDIR)/,$(DEPDIR)/,$(DSEKAI_O_FILES_SDL:.o=.d))

# ====== Main: xlib ======

# 1. Directories

OBJDIR_XLIB := $(OBJDIR)/xlib
DEPDIR_XLIB := $(DEPDIR)/xlib
GENDIR_XLIB := $(GENDIR)/xlib

# 2. Files

DSEKAI_C_FILES_XLIB_ONLY := \
   src/main.c \
   unilayer/input/xi.c \
   unilayer/graphics/xg.c \
   unilayer/memory/fakem.c

DSEKAI_ASSETS_MAPS_XLIB := \
   $(subst .json,.h,$(subst $(ASSETDIR)/,$(GENDIR_XLIB)/,$(DSEKAI_ASSETS_MAPS)))

# 3. Programs

CC_XLIB := gcc
LD_XLIB := gcc

# 4. Arguments

CFLAGS_XLIB := -DSCREEN_SCALE=3 -g -DSCREEN_W=160 -DSCREEN_H=160 -std=c89 -DPLATFORM_XLIB $(CFLAGS_DEBUG_GCC) -I$(GENDIR_XLIB) -DUSE_SOFTWARE_TEXT $(CFLAGS_OPT)

LDFLAGS_XLIB := -g -lX11 $(CFLAGS_DEBUG_GCC)

DSEKAI_O_FILES_XLIB := \
   $(addprefix $(OBJDIR_XLIB)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_XLIB)/,$(subst .c,.o,$(DSEKAI_C_FILES_XLIB_ONLY)))

# 5. Targets

ifeq ($(RESOURCE),"FILE")
$(GENDIR_XLIB)/resext.h: \
$(DSEKAI_ASSETS_BITMAPS_16x16x4) $(DSEKAI_ASSETS_MAPS) | \
$(GENDIR_XLIB)/$(STAMPFILE) $(HEADPACK)
else ifeq ($(RESOURCE),"HEADER")
$(GENDIR_XLIB)/resext.h: \
$(DSEKAI_ASSETS_BITMAPS_16x16x4) $(DSEKAI_ASSETS_MAPS) | \
$(GENDIR_XLIB)/$(STAMPFILE) $(MKRESH)
	$(MKRESH) -f file -if $^ -oh $@
endif

$(BIN_XLIB): $(DSEKAI_O_FILES_XLIB) | $(BINDIR) $(DSEKAI_ASSETS_MAPS_XLIB) $(GENDIR_XLIB)/resext.h
	$(LD_XLIB) -o $@ $^ $(LDFLAGS_XLIB)

$(OBJDIR_XLIB)/%.o: %.c $(GENDIR_XLIB)/resext.h | $(DSEKAI_ASSETS_MAPS_XLIB)
	$(MD) $(dir $@)
	$(CC_XLIB) $(CFLAGS_XLIB) -c -o $@ $(<:%.o=%)

$(DEPDIR_XLIB)/%.d: %.c $(GENDIR_XLIB)/resext.h | $(DSEKAI_ASSETS_MAPS_XLIB)
	$(MD) $(dir $@)
	$(CC_XLIB) $(CFLAGS_XLIB) -MM $< \
      -MT $(subst .c,.o,$(addprefix $(DEPDIR_XLIB)/,$<)) -MF $@

#include $(subst $(OBJDIR)/,$(DEPDIR)/,$(DSEKAI_O_FILES_XLIB:.o=.d))

# ====== Main: MS-DOS ======

# 1. Directories

OBJDIR_DOS := $(OBJDIR)/dos
DEPDIR_DOS := $(DEPDIR)/dos
GENDIR_DOS := $(GENDIR)/dos

# 2. Files

DSEKAI_C_FILES_DOS_ONLY := \
   src/main.c \
   unilayer/input/dosi.c \
   unilayer/graphics/dosg.c \
   unilayer/memory/fakem.c

DSEKAI_ASSETS_MAPS_DOS := \
   $(subst .json,.h,$(subst $(ASSETDIR)/,$(GENDIR_DOS)/,$(DSEKAI_ASSETS_MAPS)))

DSEKAI_ASSETS_DOS_CGA := \
   $(subst .bmp,.cga,$(subst $(ASSETDIR)/,$(GENDIR_DOS)/,$(DSEKAI_ASSETS_BITMAPS_16x16x4)))

DSEKAI_O_FILES_DOS := \
   $(addprefix $(OBJDIR_DOS)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_DOS)/,$(subst .c,.o,$(DSEKAI_C_FILES_DOS_ONLY)))

# 3. Programs

CC_DOS := wcc
LD_DOS := wcl

# 4. Arguments

CFLAGS_DOS := -hw -d3 -0 -ms -DPLATFORM_DOS -DUSE_LOOKUPS -zp=1 -DSCREEN_W=320 -DSCREEN_H=200 -i=$(GENDIR_DOS) -DUSE_SOFTWARE_TEXT $(CFLAGS_OPT)

LDFLAGS_DOS := $(CFLAGS_DOS)

# 5. Targets

ifeq ($(RESOURCE),"FILE")
$(GENDIR_DOS)/resext.h: \
$(DSEKAI_ASSETS_DOS_CGA) $(DSEKAI_ASSETS_MAPS) | \
$(GENDIR_DOS)/$(STAMPFILE) $(HEADPACK)
else ifeq ($(RESOURCE),"HEADER")
$(GENDIR_DOS/resext.h: \
$(DSEKAI_ASSETS_DOS_CGA) $(DSEKAI_ASSETS_MAPS) | \
$(GENDIR_DOS)/$(STAMPFILE) $(MKRESH)
	$(MKRESH) -f file -if $^ -oh $@
endif

#$(BINDIR)/doscga.drc: res_doscga_drc

#res_doscga_drc: $(DRCPACK) $(DSEKAI_ASSETS_DOS_CGA)
#	rm $(BINDIR)/doscga.drc || true
#	$(DRCPACK) -c -a -af $(BINDIR)/doscga.drc -i 5001 \
#      -if $(GENDIR_DOS)/*.cga $(DSEKAI_ASSETS_MAPS) -lh $(GENDIR_DOS)/resext.h

$(GENDIR_DOS)/%.cga: $(ASSETDIR)/%.bmp $(CONVERT) | $(GENDIR_DOS)/$(STAMPFILE)
	$(MD) $(dir $@)
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
   unilayer/input/palmi.c \
   unilayer/memory/palmm.c \
   unilayer/resource/palmr.c \
   unilayer/graphics/palmg.c

DSEKAI_ASSETS_PALM := \
   $(subst $(ASSETDIR)/,$(GENDIR_PALM)/,$(DSEKAI_ASSETS_BITMAPS_16x16x4))

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

CFLAGS_PALM := -Os -DSCREEN_W=160 -DSCREEN_H=160 -I /opt/palmdev/sdk-3.5/include -I /opt/palmdev/sdk-3.5/include/Core/UI/ -I /opt/palmdev/sdk-3.5/include/Core/System/ -I /opt/palmdev/sdk-3.5/include/Core/Hardware/ -I /opt/palmdev/sdk-3.5/include/Core/International/ -DPLATFORM_PALM -g $(CFLAGS_DEBUG_GENERIC) -DUSE_JSON_MAPS $(CFLAGS_OPT)

LDFLAGS_PALM = -g $(CFLAGS_PALM)

APPID := DSEK

# 5. Targets

$(GENDIR_PALM)/resext.h: \
$(DSEKAI_ASSETS_BITMAPS_16x16x4) $(DSEKAI_ASSETS_MAPS) | \
$(GENDIR_PALM)/$(STAMPFILE) $(HEADPACK)

$(GENDIR_PALM)/%.bmp: $(ASSETDIR)/%.bmp $(CONVERT) | $(GENDIR_PALM)/$(STAMPFILE)
	$(MD) $(dir $@)
	$(CONVERT) -if $< -of $@ -ob 1 -r -ic bitmap -oc bitmap

$(GENDIR_PALM)/resext.h \
$(GENDIR_PALM)/palmd.rcp: \
$(DSEKAI_ASSETS_PALM) $(DSEKAI_ASSETS_MAPS) | $(GENDIR_PALM)/$(STAMPFILE) $(MKRESH)
	$(MKRESH) -f palm -i 5001 \
      -if $(DSEKAI_ASSETS_PALM) $(DSEKAI_ASSETS_MAPS) \
      -oh $(GENDIR_PALM)/resext.h \
      -or $(GENDIR_PALM)/palmd.rcp

grc_palm: $(OBJDIR_PALM)/$(DSEKAI)
	cd $(OBJDIR_PALM) && $(OBJRES) $(DSEKAI)

$(OBJDIR_PALM)/$(DSEKAI): $(DSEKAI_O_FILES_PALM)
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

# 1. Directories

OBJDIR_WIN16 := $(OBJDIR)/win16
DEPDIR_WIN16 := $(DEPDIR)/win16
GENDIR_WIN16 := $(GENDIR)/win16

# 2. Files

DSEKAI_C_FILES_WIN16_ONLY := \
   src/main.c \
   unilayer/input/wini.c \
   unilayer/resource/winr.c \
   unilayer/memory/winm.c \
   unilayer/graphics/wing.c

DSEKAI_ASSETS_MAPS_WIN16 := \
   $(subst .json,.h,$(subst $(ASSETDIR)/,$(GENDIR_WIN16)/,$(DSEKAI_ASSETS_MAPS)))

DSEKAI_O_FILES_WIN16 := \
   $(addprefix $(OBJDIR_WIN16)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_WIN16)/,$(subst .c,.o,$(DSEKAI_C_FILES_WIN16_ONLY)))

# 3. Programs

CC_WIN16 := wcc
LD_WIN16 := wcl
RC_WIN16 := wrc

# 4. Arguments

CFLAGS_WIN16 := -bt=windows -i=$(INCLUDE)/win -bw -DSCREEN_SCALE=2 -DPLATFORM_WIN16 $(CFLAGS_DEBUG_GENERIC) -zp=1 -DSCREEN_W=160 -DSCREEN_H=160 -DUSE_SOFTWARE_TEXT $(CFLAGS_OPT)

LDFLAGS_WIN16 := -l=windows -zp=1

RCFLAGS_WIN16 := -r -DPLATFORM_WIN16 -i $(INCLUDE)win

# 5. Targets

$(GENDIR_WIN16)/resext.h: \
$(DSEKAI_ASSETS_BITMAPS_16x16x4) $(DSEKAI_ASSETS_MAPS) | \
$(GENDIR_WIN16)/$(STAMPFILE) $(HEADPACK)

$(BINDIR)/$(DSEKAI)16.img: $(BIN_WIN16)
	$(DD) if=/dev/zero bs=512 count=2880 of="$@"
	$(MKFSVFAT) "$@"
	$(MCOPY) -i "$@" $< ::$(DSEKAI)16.exe

$(BIN_WIN16): \
$(DSEKAI_O_FILES_WIN16) $(OBJDIR_WIN16)/win16.res | \
$(BINDIR)/$(STAMPFILE) $(DSEKAI_ASSETS_MAPS_WIN16)
	$(LD_WIN16) $(LDFLAGS_WIN16) -fe=$@ $^

$(OBJDIR_WIN16)/win16.res: \
src/winstat.rc $(GENDIR_WIN16)/win16.rc $(ASSETDIR)/$(DSEKAI).ico | \
$(OBJDIR_WIN16)/$(STAMPFILE)
	$(RC_WIN16) $(RCFLAGS_WIN16) $< -o $@

$(GENDIR_WIN16)/win16.rc \
$(GENDIR_WIN16)/resext.h: $(DSEKAI_ASSETS_BITMAPS_16x16x4) | \
$(MKRESH) $(GENDIR_WIN16)/$(STAMPFILE)
	$(MKRESH) -f win16 -i 5001 \
      -if $^ \
      -oh $(GENDIR_WIN16)/resext.h -or $(GENDIR_WIN16)/win16.rc

$(OBJDIR_WIN16)/%.o: \
%.c $(OBJDIR_WIN16)/win16.res $(GENDIR_WIN16)/resext.h $(DSEKAI_ASSETS_MAPS_WIN16)
	$(MD) $(dir $@)
	$(CC_WIN16) $(CFLAGS_WIN16) -fo=$@ $(<:%.c=%)

$(DEPDIR_WIN16)/%.d: %.c $(GENDIR_WIN16)/resext.h $(DSEKAI_ASSETS_MAPS_WIN16)
	$(MD) $(dir $@)
	$(HOST_CC) -DPLATFORM_WIN16 -MM $< \
      -MT $(subst .c,.o,$(addprefix $(DEPDIR_WIN16)/,$<)) -MF $@ || touch $@

#include $(subst $(OBJDIR)/,$(DEPDIR)/,$(DSEKAI_O_FILES_WIN16:.o=.d))

# ====== Main: Win32 ======

# 1. Directories

OBJDIR_WIN32 := $(OBJDIR)/win32
DEPDIR_WIN32 := $(DEPDIR)/win32
GENDIR_WIN32 := $(GENDIR)/win32

# 2. Files

DSEKAI_C_FILES_WIN32_ONLY := \
   src/main.c \
   unilayer/input/wini.c \
   unilayer/resource/winr.c \
   unilayer/memory/winm.c \
   unilayer/graphics/wing.c

DSEKAI_ASSETS_MAPS_WIN32 := \
   $(subst .json,.h,$(subst $(ASSETDIR)/,$(GENDIR_WIN32)/,$(DSEKAI_ASSETS_MAPS)))

DSEKAI_O_FILES_WIN32 := \
   $(addprefix $(OBJDIR_WIN32)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_WIN32)/,$(subst .c,.o,$(DSEKAI_C_FILES_WIN32_ONLY)))

# 3. Programs

CC_WIN32 := wcc386
LD_WIN32 := wcl386
RC_WIN32 := wrc

# 4. Arguments

CFLAGS_WIN32 := -bt=nt -3 -i=$(INCLUDE) -i=$(INCLUDE)/nt -DSCREEN_SCALE=2 -DPLATFORM_WIN32 $(CFLAGS_DEBUG_GENERIC) -zp=1 -DSCREEN_W=160 -DSCREEN_H=160 -DUSE_SOFTWARE_TEXT $(CFLAGS_OPT)

LDFLAGS_WIN32 := -bcl=nt_win -zp=1

RCFLAGS_WIN32 := -r -DPLATFORM_WIN32 -i $(INCLUDE)win

# 5. Targets

$(GENDIR_WIN32)/resext.h: \
$(DSEKAI_ASSETS_BITMAPS_16x16x4) $(DSEKAI_ASSETS_MAPS) | \
$(GENDIR_WIN32)/$(STAMPFILE) $(HEADPACK)

$(BIN_WIN32): \
$(DSEKAI_O_FILES_WIN32) $(OBJDIR_WIN32)/win32.res | \
$(BINDIR)/$(STAMPFILE) $(DSEKAI_ASSETS_MAPS_WIN32)
	$(LD_WIN32) $(LDFLAGS_WIN32) -fe=$@ $^

$(OBJDIR_WIN32)/win32.res: \
src/winstat.rc $(GENDIR_WIN32)/win32.rc $(ASSETDIR)/$(DSEKAI).ico | \
$(OBJDIR_WIN32)/$(STAMPFILE)
	$(RC_WIN32) $(RCFLAGS_WIN32) $< -o $@

$(GENDIR_WIN32)/win32.rc \
$(GENDIR_WIN32)/resext.h: \
$(DSEKAI_ASSETS_BITMAPS_16x16x4) | \
$(MKRESH) $(GENDIR_WIN32)/$(STAMPFILE)
	$(MKRESH) -f win16 -i 5001 \
      -if $^ \
      -oh $(GENDIR_WIN32)/resext.h -or $(GENDIR_WIN32)/win32.rc

$(OBJDIR_WIN32)/%.o: \
%.c $(OBJDIR_WIN32)/win32.res $(GENDIR_WIN32)/resext.h $(DSEKAI_ASSETS_MAPS_WIN32)
	$(MD) $(dir $@)
	$(CC_WIN32) $(CFLAGS_WIN32) -fo=$@ $(<:%.c=%)

$(DEPDIR_WIN32)/%.d: %.c $(GENDIR_WIN32)/resext.h $(DSEKAI_ASSETS_MAPS_WIN32)
	$(MD) $(dir $@)
	$(HOST_CC) -DPLATFORM_WIN32 -MM $< \
      -MT $(subst .c,.o,$(addprefix $(DEPDIR_WIN32)/,$<)) -MF $@ || touch $@

#include $(subst $(OBJDIR)/,$(DEPDIR)/,$(DSEKAI_O_FILES_WIN32:.o=.d))

# ====== Main: MacOS 7 ======

# 1. Directories

OBJDIR_MAC6 := $(OBJDIR)/mac6
DEPDIR_MAC6 := $(DEPDIR)/mac6
GENDIR_MAC6 := $(GENDIR)/mac6

RETRO68_PREFIX := /opt/Retro68-build/toolchain

# 2. Files

DSEKAI_C_FILES_MAC6_ONLY := \
   src/main.c \
   unilayer/input/mac6i.c \
   unilayer/graphics/mac6g.c \
   unilayer/memory/mac6m.c

DSEKAI_ASSETS_ICNS := \
   $(subst .bmp,.icns,$(subst $(ASSETDIR)/,$(GENDIR_MAC6)/,$(DSEKAI_ASSETS_BITMAPS_16x16x4)))

DSEKAI_ASSETS_RSRC := \
   $(subst .bmp,.rsrc,$(subst $(ASSETDIR)/,$(GENDIR_MAC6)/,$(DSEKAI_ASSETS_BITMAPS_16x16x4)))

DSEKAI_ASSETS_MAPS_MAC6 := \
   $(subst .json,.h,$(subst $(ASSETDIR)/,$(GENDIR_MAC6)/,$(DSEKAI_ASSETS_MAPS)))

DSEKAI_O_FILES_MAC6 := \
   $(addprefix $(OBJDIR_MAC6)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_MAC6)/,$(subst .c,.o,$(DSEKAI_C_FILES_MAC6_ONLY)))

# 3. Programs

CC_MAC6 := m68k-apple-macos-gcc
CXX_MAC6 := m68k-apple-macos-g++
LDFLAGS_MAC6 := -lRetroConsole
REZ_MAC6 := Rez

# 4. Arguments

CFLAGS_MAC6 := -DPLATFORM_MAC6 -I$(RETRO68_PREFIX)/multiversal/CIncludes $(CFLAGS_DEBUG_GENERIC) -DUSE_SOFTWARE_TEXT $(CFLAGS_OPT)

# 5. Targets

#$(BINDIR)/mac6.drc \
#$(GENDIR_MAC6)/resext.h: $(DSEKAI_ASSETS_PICTS) $(DRCPACK) | \
#$(GENDIR_MAC6)/$(STAMPFILE)
#	$(DRCPACK) -c -a -af $(BINDIR)/mac6.drc -t PICT -i 5001 \
#      -if $(DSEKAI_ASSETS_PICTS) $(DSEKAI_ASSETS_MAPS) \
#      -lh $(GENDIR_MAC6)/resext.h

ifeq ($(RESOURCE),"FILE")
$(GENDIR_MAC6)/resext.h: \
$(DSEKAI_ASSETS_BITMAPS_16x16x4) $(DSEKAI_ASSETS_MAPS) | \
$(GENDIR_MAC6)/$(STAMPFILE) $(HEADPACK)
else ifeq ($(RESOURCE),"HEADER")
$(GENDIR_MAC6)/resext.h: \
$(DSEKAI_ASSETS_BITMAPS_16x16x4) $(DSEKAI_ASSETS_MAPS) | \
$(GENDIR_MAC6)/$(STAMPFILE) $(MKRESH)
	$(MKRESH) -f file -if $^ -oh $@
endif

#$(GENDIR_MAC6)/%.pict: $(ASSETDIR)/%.bmp | $(GENDIR_MAC6)/$(STAMPFILE)
#	$(IMAGEMAGICK) $< $@

#$(GENDIR_MAC6)/%.rsrc: $(GENDIR_MAC6)/%.icns
#	echo "read 'icns' (-16455) \"$<\";" > $@

$(GENDIR_MAC6)/%.icns: $(ASSETDIR)/%.bmp $(CONVERT) | $(GENDIR_MAC6)/$(STAMPFILE)
	$(MD) $(dir $@)
	$(CONVERT) -if $< -of $@ -ob 1 -r -ic bitmap -oc icns

$(OBJDIR_MAC6)/$(DSEKAI).code.bin: $(DSEKAI_O_FILES_MAC6) | $(OBJDIR_MAC6)
	$(CXX_MAC6) $(LDFLAGS_MAC6) -o $@ $^ # Use CXX to link for RetroConsole.

$(BIN_MAC6): $(OBJDIR_MAC6)/$(DSEKAI).code.bin
	$(REZ_MAC6) -I$(RETRO68_PREFIX)/RIncludes \
      --copy $^ \
      "$(RETRO68_PREFIX)/RIncludes/Retro68APPL.r" \
      -t "APPL" -c "DSEK" \
      -o $(BINDIR)/$(DSEKAI).bin \
      --cc $(BINDIR)/$(DSEKAI).APPL \
      --cc $(BINDIR)/$(DSEKAI).dsk

$(OBJDIR_MAC6)/%.o: \
%.c $(DSEKAI_ASSETS_MAPS_MAC6) $(GENDIR_MAC6)/resext.h
	$(MD) $(dir $@)
	$(CC_MAC6) $(CFLAGS_MAC6) -c -o $@ $(<:%.o=%)

# ====== Main: NDS ======

# 1. Directories

OBJDIR_NDS := $(OBJDIR)/nds
DEPDIR_NDS := $(DEPDIR)/nds
GENDIR_NDS := $(GENDIR)/nds

DEVKITPATH := $(shell echo "$(DEVKITPRO)" | sed -e 's/^\([a-zA-Z]\):/\/\1/')

# 2. Files

DSEKAI_C_FILES_NDS_ONLY := \
   src/main.c \
   unilayer/input/ndsi.c \
   unilayer/graphics/ndsg.c \
   unilayer/memory/fakem.c

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

CFLAGS_NDS := --sysroot $(DEVKITARM)/arm-none-eabi -I$(DEVKITPRO)/libnds/include -DPLATFORM_NDS -DARM9 -g -march=armv5te -mtune=arm946e-s -fomit-frame-pointer -ffast-math $(ARCH_NDS) -DUSE_SOFTWARE_TEXT $(CFLAGS_OPT)

LIBS_NDS := -L$(DEVKITPRO)/libnds/lib -lnds9

LDFLAGS_NDS := -specs=ds_arm9.specs -g $(ARCH_NDS) -Wl,-Map,$(OBJDIR_NDS)/$(DSEKAI).map

$(BIN_NDS): PATH := $(DEVKITPATH)/tools/bin:$(DEVKITPATH)/devkitARM/bin:$(PATH)

# 5. Targets

ifeq ($(RESOURCE),"FILE")
$(GENDIR_NDS)/resext.h: \
$(DSEKAI_ASSETS_BITMAPS_16x16x4) $(DSEKAI_ASSETS_MAPS) | \
$(GENDIR_NDS)/$(STAMPFILE) $(HEADPACK)
else ifeq ($(RESOURCE),"HEADER")
$(GENDIR_NDS)/resext.h: \
$(DSEKAI_ASSETS_BITMAPS_16x16x4) $(DSEKAI_ASSETS_MAPS) | \
$(GENDIR_NDS)/$(STAMPFILE) $(MKRESH)
	$(MKRESH) -f file -if $^ -oh $@
endif

$(BIN_NDS): $(OBJDIR_NDS)/$(DSEKAI).elf $(GENDIR_NDS)/$(DSEKAI)-1.bmp
	$(NDSTOOL) -c $@ -9 $< -b $(GENDIR_NDS)/$(DSEKAI)-1.bmp "$(DSEKAI);$(DSEKAI);$(DSEKAI)"

$(GENDIR_NDS)/$(DSEKAI)-1.bmp: $(ASSETDIR)/$(DSEKAI).ico
	$(IMAGEMAGICK) $< -compress none -colors 16 $(GENDIR_NDS)/$(DSEKAI).bmp

$(OBJDIR_NDS)/$(DSEKAI).elf: $(DSEKAI_O_FILES_NDS)
	$(LD_NDS) $(LDFLAGS_NDS) $^ $(LIBS_NDS) -o $@

$(OBJDIR_NDS)/%.o: \
%.c $(DSEKAI_ASSETS_MAPS_NDS) $(GENDIR_NDS)/resext.h
	$(MD) $(dir $@)
	$(CC_NDS) $(CFLAGS_NDS) -c -o $@ $(<:%.o=%)

$(DEPDIR_NDS)/%.d: %.c $(GENDIR_NDS)/resext.h $(DSEKAI_ASSETS_MAPS_NDS)
	$(MD) $(dir $@)
	$(CC_NDS) $(CFLAGS_NDS) -MM $< \
      -MT $(subst .c,.o,$(addprefix $(DEPDIR_NDS)/,$<)) -MF $@ || touch $@

#include $(subst $(OBJDIR)/,$(DEPDIR)/,$(DSEKAI_O_FILES_NDS:.o=.d))

# ====== Main: emscripten ======

# 1. Directories

OBJDIR_WEB := $(OBJDIR)/web
DEPDIR_WEB := $(DEPDIR)/web
GENDIR_WEB := $(GENDIR)/web

# 2. Files

DSEKAI_C_FILES_WEB_ONLY := \
   src/main.c \
   unilayer/input/gli.c \
   unilayer/graphics/glg.c \
   unilayer/memory/fakem.c \
   src/json.c

DSEKAI_O_FILES_WEB := \
   $(addprefix $(OBJDIR_WEB)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_WEB)/,$(subst .c,.o,$(DSEKAI_C_FILES_WEB_ONLY)))

# 3. Programs

CC_WEB := emcc
LD_WEB := emcc

# 4. Arguments

CFLAGS_WEB := -DSCREEN_SCALE=3 -DSCREEN_W=160 -DSCREEN_H=160 -std=c89 -DPLATFORM_WEB -DUSE_JSON_MAPS -DUSE_SOFTWARE_TEXT $(CFLAGS_OPT) -DRESOURCE_HEADER

LDFLAGS_WEB :=

# 5. Targets

$(GENDIR_WEB)/resext.h: \
$(DSEKAI_ASSETS_BITMAPS_16x16x4) $(DSEKAI_ASSETS_MAPS) | \
$(GENDIR_WEB)/$(STAMPFILE) $(HEADPACK)

$(BIN_WEB): $(DSEKAI_O_FILES_WEB) | $(BINDIR)/$(STAMPFILE)
	$(LD_WEB) -o $@ $^ $(LDFLAGS_WEB)

$(OBJDIR_WEB)/%.o: %.c $(GENDIR_WEB)/resext.h | $(DSEKAI_ASSETS_MAPS)
	$(MD) $(dir $@)
	$(CC_WEB) $(CFLAGS_WEB) -c -o $@ $(<:%.o=%)

$(DEPDIR_WEB)/%.d: %.c $(GENDIR_WEB)/resext.h
	$(MD) $(dir $@)
	$(CC_WEB) $(CFLAGS_WEB) -MM $< \
      -MT $(subst .c,.o,$(addprefix $(DEPDIR_WEB)/,$<)) -MF $@

#include $(subst $(OBJDIR)/,$(DEPDIR)/,$(DSEKAI_O_FILES_WEB:.o=.d))

# ====== Main: Curses ======

# 1. Directories

OBJDIR_CURSES := $(OBJDIR)/curses
DEPDIR_CURSES := $(DEPDIR)/curses
GENDIR_CURSES := $(GENDIR)/curses

# 2. Files

DSEKAI_C_FILES_CURSES_ONLY := \
   src/main.c \
   unilayer/input/cursesi.c \
   unilayer/graphics/cursesg.c \
   unilayer/memory/fakem.c \
   src/json.o

DSEKAI_O_FILES_CURSES := \
   $(addprefix $(OBJDIR_CURSES)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_CURSES)/,$(subst .c,.o,$(DSEKAI_C_FILES_CURSES_ONLY)))

# 3. Programs

CC_CURSES := gcc
LD_CURSES := gcc

# 4. Arguments

CFLAGS_CURSES := $(shell pkg-config ncurses --cflags) -g -DSCREEN_W=160 -DSCREEN_H=160 -std=c89 -DPLATFORM_CURSES $(CFLAGS_DEBUG_GCC) -DUSE_JSON_MAPS $(CFLAGS_OPT)

LDFLAGS_CURSES := $(shell pkg-config ncurses --libs) -g $(CFLAGS_DEBUG_GCC)

# 5. Targets

ifeq ($(RESOURCE),"FILE")
$(GENDIR_CURSES)/resext.h: \
$(DSEKAI_ASSETS_BITMAPS_16x16x4) $(DSEKAI_ASSETS_MAPS) | \
$(GENDIR_CURSES)/$(STAMPFILE) $(HEADPACK)
else ifeq ($(RESOURCE),"HEADER")
$(GENDIR_CURSES)/resext.h: \
$(DSEKAI_ASSETS_BITMAPS_16x16x4) $(DSEKAI_ASSETS_MAPS) | \
$(GENDIR_CURSES)/$(STAMPFILE) $(MKRESH)
	$(MKRESH) -f file -if $^ -oh $@
endif

$(BIN_CURSES): $(DSEKAI_O_FILES_CURSES) | $(BINDIR)/$(STAMPFILE)
	$(LD_CURSES) -o $@ $^ $(LDFLAGS_CURSES)

$(OBJDIR_CURSES)/%.o: %.c $(GENDIR_CURSES)/resext.h | $(DSEKAI_ASSETS_MAPS)
	$(MD) $(dir $@)
	$(CC_CURSES) $(CFLAGS_CURSES) -c -o $@ $(<:%.o=%)

$(DEPDIR_CURSES)/%.d: %.c $(GENDIR_CURSES)/resext.h
	$(MD) $(dir $@)
	$(CC_CURSES) $(CFLAGS_CURSES) -MM $< \
      -MT $(subst .c,.o,$(addprefix $(DEPDIR_CURSES)/,$<)) -MF $@

#include $(subst $(OBJDIR)/,$(DEPDIR)/,$(DSEKAI_O_FILES_CURSES:.o=.d))

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

#include $(subst $(OBJDIR)/,$(DEPDIR)/,$(DSEKAI_O_FILES_CHECK_NULL:.o=.d))
	
$(OBJDIR_CHECK_NULL)/%.o: %.c check/testdata.h $(GENDIR_CHECK_NULL)/resext.h
	$(MD) $(dir $@)
	$(CC) $(CFLAGS_CHECK_NULL) -c -o $@ $(<:%.o=%)

# ====== Clean ======

clean:
	rm -rf data obj bin gen *.err .rsrc .finf

