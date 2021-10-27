
DSEKAI := dsekai

ENTRY_MAP := field

DSEKAI_C_FILES := \
   src/tilemap.c \
   unilayer/graphics.c \
   src/mobile.c \
   src/item.c \
   src/window.c \
   src/script.c \
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
   unilayer/resource/file.c \
   unilayer/memory/fakem.c \
   unilayer/dio.c

HEADPACK_C_FILES := \
   tools/headpack.c \
   tools/map2h.c \
   src/tmjson.c \
   src/tilemap.c \
   src/mobile.c \
   src/script.c \
   src/json.c \
   unilayer/resource/file.c \
   unilayer/graphics.c \
   unilayer/graphics/nullg.c \
   unilayer/memory/fakem.c \
   unilayer/dio.c

DRCPACK_C_FILES := \
   tools/drcpack.c \
   tools/data/drcwrite.c \
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
   tools/data/icns.c \
   src/json.c

LOOKUPS_C_FILES := \
   tools/lookups.c

MAP2ASN_C_FILES := \
   tools/map2asn.c \
   src/tilemap.c \
   src/mobile.c \
   src/script.c \
   src/json.c \
   src/tmjson.c \
   unilayer/resource/file.c \
   unilayer/graphics.c \
   unilayer/graphics/nullg.c \
   unilayer/memory/fakem.c \
   unilayer/dio.c

MAP2BIN_C_FILES := \
   tools/map2bin.c \
   src/tilemap.c \
   src/mobile.c \
   src/script.c \
   src/json.c \
   src/tmjson.c \
   unilayer/resource/file.c \
   unilayer/graphics.c \
   unilayer/graphics/nullg.c \
   unilayer/memory/fakem.c \
   unilayer/dio.c

# ALL platforms.
PLATFORMS := sdl xlib dos win16 win32 palm mac6 nds curses check_null

# Platforms on which RESOURCE=FILE is working.
PLATFORMS_FILE := sdl xlib win32

ASSETDIR := assets
ASSETPATH :=
DTHRESHOLD := 3

ifeq ($(RESOURCE),FILE)

   OBJDIR := obj-file
   DEPDIR := dep-file
   GENDIR := gen-file
   BINDIR := bin-file

else

   OBJDIR := obj
   DEPDIR := dep
   GENDIR := gen
   BINDIR := bin

endif

OBJDIR_CHECK_NULL := $(OBJDIR)/check_null

DEPDIR_CHECK_NULL := $(DEPDIR)/check_null

GENDIR_CHECK_NULL := $(GENDIR)/check_null

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
BIN_SDL_ARM := $(BINDIR)/$(DSEKAI)r
BIN_MEGAD := $(BINDIR)/$(DSEKAI).smd

BIN_CHECK_NULL := $(BINDIR)/check

PKGDIR := packages
PKGOS := $(shell uname -s -m | sed 's/ /-/g' | tr '[:upper:]' '[:lower:]'])

GIT_HASH := $(shell git log -1 --pretty=format:"%h")

PKG_OUT_FLAGS :=

DEFINES_DSEKAI := -DUNILAYER_PROJECT_NAME=\"dsekai\" -DDSEKAI_GIT_HASH=\"$(GIT_HASH)\"

ifeq ($(RESOURCE),FILE)

   DEFINES_RESOURCE := -DRESOURCE_FILE -DASSETS_PATH="\"$(ASSETPATH)\""
   DSEKAI_C_FILES_RES := unilayer/resource/file.c src/json.c
   PKG_OUT_FLAGS := $(PKG_OUT_FLAGS)-file

   ifeq ($(FMT_ASN),TRUE)

all: $(BIN_DOS) $(BIN_SDL) $(BIN_XLIB) $(BIN_WIN32) $(BIN_WIN16)

   else

all: $(BIN_SDL) $(BIN_XLIB) $(BIN_WIN32)

   endif

else

   RESOURCE := DEFAULT
   DEFINES_RESOURCE := -DRESOURCE_HEADER
   DSEKAI_C_FILES_RES := unilayer/resource/header.c

all: $(BIN_DOS) $(BIN_SDL) $(BIN_XLIB) $(BIN_WIN16) $(BIN_WIN32)

endif

CFLAGS_GCC_GENERIC := -Iunilayer
CFLAGS_OWC_GENERIC :=

ifeq ($(BUILD),RELEASE)

   LDFLAGS_OWC_GENERIC :=
   SANITIZE := NO
   PKG_OUT_FLAGS := $(PKG_OUT_FLAGS)-release

else

   # Assume debug build.

   CFLAGS_GCC_GENERIC += -Wall -Wno-missing-braces -Wno-char-subscripts -pg -g -Os
   DEFINES_DSEKAI += -DDEBUG_LOG -DDEBUG_THRESHOLD=$(DTHRESHOLD)
   LDFLAGS_GCC_GENERIC := -g -pg -Os
   PKG_OUT_FLAGS := $(PKG_OUT_FLAGS)-debug

endif

ifeq ($(SANITIZE),NO)
   FLAGS_GCC_SANITIZE := 
else
   FLAGS_GCC_SANITIZE := -fsanitize=address -fsanitize=leak -fsanitize=undefined
endif

TILEMAP_FMTS :=

ifeq ($(RESOURCE),FILE)

   ifeq ($(FMT_ASN),TRUE)
      DEFINES_DSEKAI += -DTILEMAP_FMT_ASN
      TILEMAP_FMTS += BIN
      DSEKAI_C_FILES_RES += src/tmasn.c
      PKG_OUT_FLAGS := $(PKG_OUT_FLAGS)-asn
   endif

   ifeq ($(FMT_JSON),TRUE)
      DEFINES_DSEKAI += -DTILEMAP_FMT_JSON
      DSEKAI_C_FILES_RES += src/tmjson.c
      TILEMAP_FMTS += JSON
      PKG_OUT_FLAGS := $(PKG_OUT_FLAGS)-json
   endif

endif

ifeq ($(ARCFMT),ZIP)
   PKG_OUT_EXT := .zip
else
   PKG_OUT_EXT := .tar.gz
endif

define BITMAPS_RULE
DSEKAI_ASSETS_SPRITES_$(DEPTH) := \
   $(wildcard $(ASSETDIR)/$(DEPTH)/s_*.bmp)
DSEKAI_ASSETS_TILES_$(DEPTH) := \
   $(wildcard $(ASSETDIR)/$(DEPTH)/t_*.bmp)
DSEKAI_ASSETS_PATTERNS_$(DEPTH) := \
   $(wildcard $(ASSETDIR)/$(DEPTH)/p_*.bmp)
DSEKAI_ASSETS_BITMAPS_$(DEPTH) := \
   $(wildcard $(ASSETDIR)/$(DEPTH)/s_*.bmp) \
   $(wildcard $(ASSETDIR)/$(DEPTH)/t_*.bmp) \
   $(wildcard $(ASSETDIR)/$(DEPTH)/p_*.bmp)
endef

DEPTHS := 16x16x4

$(foreach DEPTH,$(DEPTHS), $(eval $(BITMAPS_RULE)))

DSEKAI_ASSETS_MAPS_JSON := \
   $(ASSETDIR)/m_field.json

DSEKAI_ASSETS_TILESETS_JSON := \
   $(ASSETDIR)/t2_field.json

DSEKAI_ASSETS_MAPS_ASN := $(subst .json,.asn,$(DSEKAI_ASSETS_MAPS_JSON))

HOST_CC := gcc
MD := mkdir -p
DD := /bin/dd
MCOPY := mcopy
MKFSVFAT := /sbin/mkfs.vfat
IMAGEMAGICK := convert
PYTHON := python3
TAR := tar
GZIP := gzip
ZIP := zip

MKRESH := $(BINDIR)/mkresh
DRCPACK := $(BINDIR)/drcpack
CONVERT := $(BINDIR)/convert
LOOKUPS := $(BINDIR)/lookups
HEADPACK := $(BINDIR)/headpack
MAP2ASN := $(BINDIR)/map2asn
MAP2BIN := $(BINDIR)/map2bin

CFLAGS_MKRESH := -DNO_RESEXT -g -DDEBUG_LOG -DDEBUG_THRESHOLD=0 -DRESOURCE_FILE -Iunilayer -DASSETS_PATH="\"$(ASSETPATH)\""
CFLAGS_DRCPACK := -DNO_RESEXT -g -DDRC_READ_WRITE -DDEBUG_LOG -DDEBUG_THRESHOLD=3 -DRESOURCE_DRC -Iunilayer
CFLAGS_CONVERT := -DNO_RESEXT -g -DRESOURCE_FILE -Iunilayer
CFLAGS_LOOKUPS := -g -Iunilayer
CFLAGS_HEADPACK := -g -Iunilayer -DNO_RESEXT -DDEBUG_THRESHOLD=3 -DRESOURCE_FILE -DASSETS_PATH="\"$(ASSETPATH)\""
CFLAGS_MAP2ASN := -g -Iunilayer -DNO_RESEXT -DDEBUG_THRESHOLD=3 -DRESOURCE_FILE -DASSETS_PATH="\"$(ASSETPATH)\""
CFLAGS_MAP2BIN := -g -Iunilayer -DNO_RESEXT -DDEBUG_THRESHOLD=3 -DRESOURCE_FILE -DASSETS_PATH="\"$(ASSETPATH)\""

CFLAGS_CHECK_NULL := -DSCREEN_SCALE=3 $(shell pkg-config check --cflags) -g -DSCREEN_W=160 -DSCREEN_H=160 -std=c89 -DPLATFORM_NULL $(CFLAGS_GCC_GENERIC) -DRESOURCE_DRC

$(BIN_CHECK_NULL): LDFLAGS := $(shell pkg-config check --libs) -g $(LDFLAGS_GCC_GENERIC)

DSEKAI_C_FILES_CHECK_NULL := $(DSEKAI_C_FILES) $(DSEKAI_C_FILES_CHECK_NULL_ONLY)

DSEKAI_O_FILES_CHECK_NULL := \
   $(addprefix $(OBJDIR_CHECK_NULL)/,$(subst .c,.o,$(DSEKAI_C_FILES_CHECK_NULL)))

.PHONY: clean grc_palm

STAMPFILE := .stamp

# ====== Generic Rules ======

$(ASSETDIR)/%.asn: $(ASSETDIR)/%.json | $(MAP2ASN)
	$(MAP2ASN) $< $@

$(BINDIR)/$(ASSETDIR)/$(STAMPFILE):
	$(MD) $(dir $@)
	touch $@

$(BINDIR)/$(ASSETDIR)/%.bmp: \
$(ASSETDIR)/%.bmp | $(BINDIR)/$(ASSETDIR)/$(STAMPFILE)
	$(MD) $(dir $@)
	cp $^ $@

$(BINDIR)/$(ASSETDIR)/%.json: \
$(ASSETDIR)/%.json | $(BINDIR)/$(ASSETDIR)/$(STAMPFILE)
	$(MD) $(dir $@)
	cp $^ $@

$(BINDIR)/$(ASSETDIR)/%.asn: \
$(ASSETDIR)/%.json | $(BINDIR)/$(ASSETDIR)/$(STAMPFILE) $(MAP2ASN)
	$(MAP2ASN) $< $@

$(PKGDIR)/$(STAMPFILE):
	$(MD) $(dir $@)
	touch $@

$(BINDIR)/$(STAMPFILE):
	$(MD) $(BINDIR)
	touch $@

$(OBJDIR)/%/$(STAMPFILE):
	$(MD) $(dir $@)
	touch $@

$(GENDIR)/%/$(STAMPFILE):
	$(MD) $(dir $@)
	touch $@

define RESEXT_H_RULE

   ifeq ($$(RESOURCE),HEADER)

      # Header was specified explicitly.

      $(GENDIR)/$(platform)/resemb.h: $(res_gfx) $(res_maps) | \
      $(GENDIR)/$(platform)/$(STAMPFILE) $(HEADPACK)
			$(HEADPACK) $$@ $$^

      $(GENDIR)/$(platform)/resext.h: $(GENDIR)/$(platform)/resemb.h
			echo '#include "resemb.h"' > $$@

   else ifeq ($(RESOURCE),FILE)

      # File was specified explicitly.

      #$(GENDIR)/$(platform)/residx.h: $(res_gfx) $(res_maps) | \
      #$(GENDIR)/$(platform)/$(STAMPFILE) $(MKRESH)
		#	$(MKRESH) -f file -s bmp jsn json cga -if $$^ -oh $$@

      $(GENDIR)/$(platform)/resext.h: $(GENDIR)/$(platform)/$(STAMPFILE) $(res_gfx) $(res_maps)
			#echo '#include "residx.h"' > $$@
			touch $$@

   else ifeq ($(RESOURCE)$(findstring win,$(platform)),DEFAULTwin)

      # For Windows, default to Windows resources for images.

      $(GENDIR)/$(platform)/win.rc \
      $(GENDIR)/$(platform)/residx.h: $(res_gfx) | \
      $(MKRESH) $(GENDIR)/$(platform)/$(STAMPFILE)
			$(MKRESH) -f win16 -i 5001 -s bmp cga \
            -if $$^ \
            -oh $(GENDIR)/$(platform)/residx.h \
            -or $(GENDIR)/$(platform)/win.rc

      # Embed maps in preparsed structs.

      $(GENDIR)/$(platform)/resemb.h: $(res_maps) | \
      $(GENDIR)/$(platform)/$(STAMPFILE) $(HEADPACK)
			$(HEADPACK) $$@ $$^

      $(GENDIR)/$(platform)/resext.h: \
      $(GENDIR)/$(platform)/resemb.h $(GENDIR)/$(platform)/residx.h
			echo '#include "$(GENDIR)/$(platform)/residx.h"' > $$@
			echo '#include "resemb.h"' >> $$@
 
   else ifeq ($(RESOURCE)$(findstring palm,$(platform)),DEFAULTpalm)

      # For Palm, default to Windows resources for images.

      $(GENDIR)/$(platform)/residx.h \
      $(GENDIR)/$(platform)/palmd.rcp: $(res_gfx) | \
      $(MKRESH) $(GENDIR)/$(platform)/$(STAMPFILE)
			$(MKRESH) -f palm -i 5001 \
            -if $$^ \
            -oh $(GENDIR)/$(platform)/residx.h \
            -or $(GENDIR)/$(platform)/palmd.rcp

      # Embed maps in preparsed structs.

      $(GENDIR)/$(platform)/resemb.h: $(res_maps) | \
      $(GENDIR)/$(platform)/$(STAMPFILE) $(HEADPACK)
			$(HEADPACK) $$@ $$^

      $(GENDIR)/$(platform)/resext.h: \
      $(GENDIR)/$(platform)/resemb.h $(GENDIR)/$(platform)/residx.h
			echo '#include "residx.h"' > $$@
			echo '#include "resemb.h"' >> $$@

   else

      # For all other platforms, default to header.

      $(GENDIR)/$(platform)/resemb.h: $(res_gfx) $(res_maps) | \
      $(GENDIR)/$(platform)/$(STAMPFILE) $(HEADPACK)
			$(HEADPACK) $$@ $$^

      $(GENDIR)/$(platform)/resext.h: $(GENDIR)/$(platform)/resemb.h
			echo '#include "resemb.h"' > $$@

   endif

endef

define ICO_RULE
$(GENDIR)/$(platform)/%.ico: $(ASSETDIR)/%.bmp | \
$(GENDIR)/$(platform)/$(STAMPFILE)
	$(IMAGEMAGICK) $$< $$@
endef

$(foreach platform,$(PLATFORMS), $(eval $(ICO_RULE)))

define PKG_RULE
$(PKGDIR)/$(pkg_name)$(PKG_OUT_FLAGS).tar.gz: \
README.md $(pkg_reqs) | $(pkg_bin) $(PKGDIR)/$(STAMPFILE)
	cp $(pkg_bin) .
	$(pkg_strip) $(notdir $(pkg_bin))
	$(TAR) -cvf - $(notdir $(pkg_bin)) $$^ | $(GZIP) > $$@
	rm $(notdir $(pkg_bin))

$(PKGDIR)/$(pkg_name)$(PKG_OUT_FLAGS).zip: \
README.md $(pkg_reqs) | $(pkg_bin) $(PKGDIR)/$(STAMPFILE)
	cp $(pkg_bin) .
	$(pkg_strip) $(notdir $(pkg_bin))
	$(ZIP) -r $$@ $(notdir $(pkg_bin)) $$^
	rm $(notdir $(pkg_bin))

pkg_$(platform): $(PKGDIR)/$(pkg_name)$(PKG_OUT_FLAGS)$(PKG_OUT_EXT)
endef

# ====== Utilities ======

$(MKRESH): $(MKRESH_C_FILES) | $(BINDIR)/$(STAMPFILE)
	$(HOST_CC) $(CFLAGS_MKRESH) -o $@ $^

$(DRCPACK): $(DRCPACK_C_FILES) | $(BINDIR)/$(STAMPFILE)
	$(HOST_CC) $(CFLAGS_DRCPACK) -o $@ $^

$(CONVERT): $(CONVERT_C_FILES) | $(BINDIR)/$(STAMPFILE)
	$(HOST_CC) $(CFLAGS_CONVERT) -o $@ $^

$(LOOKUPS): $(LOOKUPS_C_FILES) | $(BINDIR)/$(STAMPFILE)
	$(HOST_CC) $(CFLAGS_LOOKUPS) -o $@ $^

$(HEADPACK): $(HEADPACK_C_FILES) | $(BINDIR)/$(STAMPFILE)
	$(HOST_CC) $(CFLAGS_HEADPACK) -o $@ $^

$(MAP2ASN): $(MAP2ASN_C_FILES) | $(BINDIR)/$(STAMPFILE)
	$(HOST_CC) $(CFLAGS_MAP2ASN) -o $@ $^

$(MAP2BIN): $(MAP2BIN_C_FILES) | $(BINDIR)/$(STAMPFILE)
	$(HOST_CC) $(CFLAGS_MAP2BIN) -o $@ $^

# ====== Main: SDL ======

# 1. Directories

OBJDIR_SDL := $(OBJDIR)/sdl
DEPDIR_SDL := $(DEPDIR)/sdl
GENDIR_SDL := $(GENDIR)/sdl

# 2. Files

DSEKAI_C_FILES_SDL_ONLY := \
   src/main.c \
   unilayer/input/sdli.c \
   unilayer/graphics/sdlg.c \
   unilayer/memory/fakem.c

DSEKAI_O_FILES_SDL := \
   $(addprefix $(OBJDIR_SDL)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_SDL)/,$(subst .c,.o,$(DSEKAI_C_FILES_SDL_ONLY))) \
   $(addprefix $(OBJDIR_SDL)/,$(subst .c,.o,$(DSEKAI_C_FILES_RES)))

BIN_SDL_ASSETS :=

ifeq ($(RESOURCE),FILE)
   SDL_ASSETS += $(DSEKAI_ASSETS_BITMAPS_16x16x4)
endif

ifeq ($(FMT_ASN),TRUE)
   SDL_ASSETS += $(DSEKAI_ASSETS_MAPS_ASN) 
endif

ifeq ($(FMT_JSON),TRUE)
   SDL_ASSETS += \
      $(DSEKAI_ASSETS_MAPS_JSON) \
      $(DSEKAI_ASSETS_TILESETS_JSON)
endif

# 3. Programs

CC_SDL := gcc
LD_SDL := gcc

# 4. Arguments

CFLAGS_SDL := -I $(GENDIR_SDL) -DSCREEN_SCALE=3 $(shell pkg-config sdl2 --cflags) -DSCREEN_W=160 -DSCREEN_H=160 -std=c89 -DPLATFORM_SDL -DUSE_SOFTWARE_TEXT $(DEFINES_RESOURCE) $(DEFINES_DSEKAI) $(CFLAGS_GCC_GENERIC) $(FLAGS_GCC_SANITIZE)

LDFLAGS_SDL := $(shell pkg-config sdl2 --libs) $(LDFLAGS_GCC_GENERIC) $(FLAGS_GCC_SANITIZE)

# 5. Targets

platform := sdl
res_gfx := $(DSEKAI_ASSETS_BITMAPS_16x16x4)
res_maps := $(DSEKAI_ASSETS_MAPS_JSON)
$(eval $(RESEXT_H_RULE))

pkg_bin := $(BIN_SDL)
pkg_strip := strip
pkg_name := $(DSEKAI)-$(platform)-$(PKGOS)-$(GIT_HASH)
pkg_reqs := $(SDL_ASSETS)
$(eval $(PKG_RULE))

$(BIN_SDL): $(DSEKAI_O_FILES_SDL) | $(BINDIR)/$(STAMPFILE) $(addprefix $(BINDIR)/,$(SDL_ASSETS))
	$(LD_SDL) -o $@ $^ $(LDFLAGS_SDL)

$(OBJDIR_SDL)/%.o: %.c $(GENDIR_SDL)/resext.h | $(DSEKAI_ASSETS_MAPS_JSON)
	$(MD) $(dir $@)
	$(CC_SDL) $(CFLAGS_SDL) -c -o $@ $(<:%.o=%)

$(DEPDIR_SDL)/%.d: %.c $(GENDIR_SDL)/resext.h
	$(MD) $(dir $@)
	$(CC_SDL) $(CFLAGS_SDL) -MM $< \
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

XLIB_ASSETS :=

ifeq ($(RESOURCE),FILE)
   XLIB_ASSETS += $(DSEKAI_ASSETS_BITMAPS_16x16x4)
endif

ifeq ($(FMT_ASN),TRUE)
   XLIB_ASSETS += $(DSEKAI_ASSETS_MAPS_ASN)
endif

ifeq ($(FMT_JSON),TRUE)
   XLIB_ASSETS += \
      $(DSEKAI_ASSETS_MAPS_JSON) \
      $(DSEKAI_ASSETS_TILESETS_JSON)
endif

# 3. Programs

CC_XLIB := gcc
LD_XLIB := gcc

# 4. Arguments

CFLAGS_XLIB := -DSCREEN_SCALE=3 -g -DSCREEN_W=160 -DSCREEN_H=160 -std=c89 -DPLATFORM_XLIB $(CFLAGS_GCC_GENERIC) -I$(GENDIR_XLIB) -DUSE_SOFTWARE_TEXT $(DEFINES_RESOURCE) $(DEFINES_DSEKAI) $(FLAGS_GCC_SANITIZE)

LDFLAGS_XLIB := -lX11 $(LDFLAGS_GCC_GENERIC) $(FLAGS_GCC_SANITIZE)

DSEKAI_O_FILES_XLIB := \
   $(addprefix $(OBJDIR_XLIB)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_XLIB)/,$(subst .c,.o,$(DSEKAI_C_FILES_XLIB_ONLY))) \
   $(addprefix $(OBJDIR_XLIB)/,$(subst .c,.o,$(DSEKAI_C_FILES_RES)))

# 5. Targets

platform := xlib
res_gfx := $(DSEKAI_ASSETS_BITMAPS_16x16x4)
res_maps := $(DSEKAI_ASSETS_MAPS_JSON)
$(eval $(RESEXT_H_RULE))

pkg_bin := $(BIN_XLIB)
pkg_strip := strip
pkg_name := $(DSEKAI)-$(platform)-$(PKGOS)-$(GIT_HASH)
pkg_reqs := $(XLIB_ASSETS)
$(eval $(PKG_RULE))

$(BIN_XLIB): $(DSEKAI_O_FILES_XLIB) | $(BIN_XLIB_ASSETS) $(BINDIR) $(GENDIR_XLIB)/resext.h
	$(LD_XLIB) -o $@ $^ $(LDFLAGS_XLIB)

$(OBJDIR_XLIB)/%.o: %.c $(GENDIR_XLIB)/resext.h
	$(MD) $(dir $@)
	$(CC_XLIB) $(CFLAGS_XLIB) -c -o $@ $(<:%.o=%)

#$(DEPDIR_XLIB)/%.d: %.c $(GENDIR_XLIB)/resext.h
#	$(MD) $(dir $@)
#	$(CC_XLIB) $(CFLAGS_XLIB) -MM $< \
#      -MT $(subst .c,.o,$(addprefix $(DEPDIR_XLIB)/,$<)) -MF $@

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

#DSEKAI_ASSETS_MAPS_DOS := \
#   $(subst .json,.h,$(subst $(ASSETDIR)/,$(GENDIR_DOS)/,$(DSEKAI_ASSETS_MAPS)))

DSEKAI_ASSETS_DOS_CGA := \
   $(subst .bmp,.cga,$(subst $(ASSETDIR)/,$(GENDIR_DOS)/,$(DSEKAI_ASSETS_BITMAPS_16x16x4)))

DSEKAI_O_FILES_DOS := \
   $(addprefix $(OBJDIR_DOS)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_DOS)/,$(subst .c,.o,$(DSEKAI_C_FILES_DOS_ONLY))) \
   $(addprefix $(OBJDIR_DOS)/,$(subst .c,.o,$(DSEKAI_C_FILES_RES)))

DOS_ASSETS :=

ifeq ($(RESOURCE),FILE)
   DOS_ASSETS += $(subst .bmp,.cga,$(DSEKAI_ASSETS_BITMAPS_16x16x4))
endif

ifeq ($(FMT_ASN),TRUE)
   DOS_ASSETS += $(DSEKAI_ASSETS_MAPS_ASN)
endif

# 3. Programs

CC_DOS := wcc
LD_DOS := wcl

# 4. Arguments

CFLAGS_DOS := -hw -d3 -0 -DPLATFORM_DOS -DUSE_LOOKUPS -zp=1 -DSCREEN_W=320 -DSCREEN_H=200 -i=$(GENDIR_DOS) -DUSE_SOFTWARE_TEXT $(DEFINES_RESOURCE) -i=unilayer $(DEFINES_DSEKAI) $(CFLAGS_OWC_GENERIC)

LDFLAGS_DOS := $(LDFLAGS_OWC_GENERIC)

ifeq ($(FMT_JSON),TRUE)
   CFLAGS_DOS += -mh
endif

# 5. Targets

platform := dos
res_gfx := $(DSEKAI_ASSETS_DOS_CGA)
res_maps := $(DSEKAI_ASSETS_MAPS_JSON)
$(eval $(RESEXT_H_RULE))

pkg_bin := $(BIN_DOS)
pkg_strip := echo
pkg_name := $(DSEKAI)-$(platform)-$(GIT_HASH)
pkg_reqs := $(DOS_ASSETS)
$(eval $(PKG_RULE))

#$(BINDIR)/doscga.drc: res_doscga_drc

#res_doscga_drc: $(DRCPACK) $(DSEKAI_ASSETS_DOS_CGA)
#	rm $(BINDIR)/doscga.drc || true
#	$(DRCPACK) -c -a -af $(BINDIR)/doscga.drc -i 5001 \
#      -if $(GENDIR_DOS)/*.cga $(DSEKAI_ASSETS_MAPS) -lh $(GENDIR_DOS)/resext.h

$(ASSETDIR)/%.cga: $(ASSETDIR)/%.bmp | $(CONVERT)
	$(CONVERT) -ic bitmap -oc cga -ob 2 -if $< -of $@ -og

$(BINDIR)/assets/%.cga: $(ASSETDIR)/%.bmp $(CONVERT) | \
$(BINDIR)/assets/$(STAMPFILE)
	$(MD) $(dir $@)
	$(CONVERT) -ic bitmap -oc cga -ob 2 -if $< -of $@ -og

$(GENDIR_DOS)/%.cga: $(ASSETDIR)/%.bmp $(CONVERT) | $(GENDIR_DOS)/$(STAMPFILE)
	$(MD) $(dir $@)
	$(CONVERT) -ic bitmap -oc cga -ob 2 -if $< -of $@ -og

$(BIN_DOS): $(DSEKAI_O_FILES_DOS) | $(BINDIR)/$(STAMPFILE) $(addprefix $(BINDIR)/,$(DOS_ASSETS))
	$(LD_DOS) $(LDFLAGS_DOS) -fe=$@ $^

$(OBJDIR_DOS)/%.o: %.c $(GENDIR_DOS)/resext.h
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
   unilayer/input/palmi.c \
   unilayer/memory/palmm.c \
   unilayer/resource/palmr.c \
   unilayer/graphics/palmg.c

DSEKAI_ASSETS_BITMAPS_PALM := \
   $(subst $(ASSETDIR)/,$(GENDIR_PALM)/,$(DSEKAI_ASSETS_BITMAPS_16x16x4))

DSEKAI_O_FILES_PALM := \
   $(addprefix $(OBJDIR_PALM)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_PALM)/,$(subst .c,.o,$(DSEKAI_C_FILES_PALM_ONLY)))

# 3. Programs

CC_PALM := m68k-palmos-gcc
LD_PALM := m68k-palmos-gcc
AS_PALM := m68k-palmos-as
PILRC := pilrc
OBJRES := m68k-palmos-obj-res
BUILDPRC := build-prc
#MULTIGEN := m68k-palmos-multigen

# 4. Arguments

CFLAGS_PALM := -DSCREEN_W=160 -DSCREEN_H=160 -I /opt/palmdev/sdk-3.5/include -I /opt/palmdev/sdk-3.5/include/Core/UI/ -I /opt/palmdev/sdk-3.5/include/Core/System/ -I /opt/palmdev/sdk-3.5/include/Core/Hardware/ -I /opt/palmdev/sdk-3.5/include/Core/International/ -DPLATFORM_PALM -Iunilayer -I$(GENDIR_PALM) $(DEFINES_DSEKAI) -Os

ifneq ($(BUILD),RELEASE)
   CFLAGS_PALM += -g
endif

LDFLAGS_PALM := $(CFLAGS_PALM)

#DEF_PALM := src/mulipalm.def

APPID := DSEK

# 5. Targets

platform := palm
res_gfx := $(DSEKAI_ASSETS_BITMAPS_PALM)
res_maps := $(DSEKAI_ASSETS_MAPS_JSON)
$(eval $(RESEXT_H_RULE))

pkg_bin := $(BIN_PALM)
pkg_strip := echo
pkg_name := $(DSEKAI)-$(platform)-$(GIT_HASH)
pkg_reqs :=
$(eval $(PKG_RULE))

$(GENDIR_PALM)/%.bmp: $(ASSETDIR)/%.bmp $(CONVERT) | $(GENDIR_PALM)/$(STAMPFILE)
	$(MD) $(dir $@)
	$(CONVERT) -if $< -of $@ -ob 1 -r -ic bitmap -oc bitmap

grc_palm: $(OBJDIR_PALM)/$(DSEKAI)
	cd $(OBJDIR_PALM) && $(OBJRES) $(DSEKAI)

$(OBJDIR_PALM)/$(DSEKAI): $(DSEKAI_O_FILES_PALM)
	#$(LD_PALM) $(LDFLAGS_PALM) $^ -o $@ $(GENDIR_PALM)/mulipalm.ld
	$(LD_PALM) $(LDFLAGS_PALM) $^ -o $@
	
$(OBJDIR_PALM)/bin$(STAMPFILE): src/palms.rcp $(GENDIR_PALM)/palmd.rcp
	$(PILRC) $< $(OBJDIR_PALM)
	touch $@

#$(OBJDIR_PALM)/mulipalm.o: $(GENDIR_PALM)/mulipalm.s
#	$(MD) $(dir $@)
#	$(AS_PALM) -o $@ $<

#$(GENDIR_PALM)/mulipalm.ld $(GENDIR_PALM)/mulipalm.s: $(DEF_PALM) | $(GENDIR_PALM)
#	$(MULTIGEN) -b $(GENDIR_PALM)/mulipalm $<

ifeq ($(BUILD),RELEASE)
$(BIN_PALM): grc_palm $(OBJDIR_PALM)/bin$(STAMPFILE) | $(BINDIR)/$(STAMPFILE)
	m68k-palmos-strip $(OBJDIR_PALM)/$(DSEKAI)
	$(BUILDPRC) $@ $(DSEKAI) $(APPID) $(OBJDIR_PALM)/*.grc $(OBJDIR_PALM)/*.bin
else
$(BIN_PALM): grc_palm $(OBJDIR_PALM)/bin$(STAMPFILE) | $(BINDIR)/$(STAMPFILE)
	#$(BUILDPRC) -o $@ $(DEF_PALM) $(OBJDIR_PALM)/$(DSEKAI) $(OBJDIR_PALM)/*.bin
	$(BUILDPRC) $@ $(DSEKAI) $(APPID) $(OBJDIR_PALM)/*.grc $(OBJDIR_PALM)/*.bin
endif

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
   unilayer/memory/winm.c \
   unilayer/graphics/wing.c

ifeq ($(RESOURCE),DEFAULT)
   DSEKAI_C_FILES_WIN16_ONLY += unilayer/resource/winr.c
else
   DSEKAI_C_FILES_WIN16_ONLY += $(DSEKAI_C_FILES_RES)
endif

DSEKAI_O_FILES_WIN16 := \
   $(addprefix $(OBJDIR_WIN16)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_WIN16)/,$(subst .c,.o,$(DSEKAI_C_FILES_WIN16_ONLY)))

WIN16_RES_FILES := src/winstat.rc
ifeq ($(RESOURCE),DEFAULT)
   WIN16_RES_FILES += $(GENDIR_WIN16)/win.rc
endif
WIN16_RES_FILES += $(ASSETDIR)/$(DSEKAI).ico

WIN16_ASSETS :=

ifeq ($(RESOURCE),FILE)
   WIN16_ASSETS += $(DSEKAI_ASSETS_BITMAPS_16x16x4)
endif

ifeq ($(FMT_ASN),TRUE)
   WIN16_ASSETS += $(DSEKAI_ASSETS_MAPS_ASN) 
endif

# 3. Programs

CC_WIN16 := wcc
LD_WIN16 := wcl
RC_WIN16 := wrc

# 4. Arguments

CFLAGS_WIN16 := -bt=windows -i=$(INCLUDE)/win -bw -DSCREEN_SCALE=2 -DPLATFORM_WIN16 $(CFLAGS_OWC_GENERIC) -zp=1 -DSCREEN_W=160 -DSCREEN_H=160 -DUSE_SOFTWARE_TEXT -i=unilayer $(DEFINES_DSEKAI) -I$(GENDIR_WIN16)

ifneq ($(RESOURCE),DEFAULT)
   CFLAGS_WIN16 += $(DEFINES_RESOURCE)
endif

LDFLAGS_WIN16 := -l=windows -zp=1 $(LDFLAGS_OWC_GENERIC)

RCFLAGS_WIN16 := -r -DPLATFORM_WIN16 -i $(INCLUDE)win

ifeq ($(RESOURCE),DEFAULT)
   RCFLAGS_WIN16 += -DRESOURCE_WIN
endif

# 5. Targets

platform := win16
res_gfx := $(DSEKAI_ASSETS_BITMAPS_16x16x4)
res_maps := $(DSEKAI_ASSETS_MAPS_JSON)
$(eval $(RESEXT_H_RULE))

pkg_bin := $(BIN_WIN16)
pkg_strip := echo
pkg_name := $(DSEKAI)-$(platform)-$(GIT_HASH)
pkg_reqs := $(WIN16_ASSETS)
$(eval $(PKG_RULE))

$(BINDIR)/$(DSEKAI)16.img: $(BIN_WIN16)
	$(DD) if=/dev/zero bs=512 count=2880 of="$@"
	$(MKFSVFAT) "$@"
	$(MCOPY) -i "$@" $< ::$(DSEKAI)16.exe

$(BIN_WIN16): \
$(DSEKAI_O_FILES_WIN16) $(OBJDIR_WIN16)/win.res | $(BINDIR)/$(STAMPFILE) $(BIN_WIN16_ASSETS)
	$(LD_WIN16) $(LDFLAGS_WIN16) -fe=$@ $^

$(OBJDIR_WIN16)/win.res: $(WIN16_RES_FILES) | $(OBJDIR_WIN16)/$(STAMPFILE)
	$(RC_WIN16) $(RCFLAGS_WIN16) $< -o $@

$(OBJDIR_WIN16)/%.o: %.c $(OBJDIR_WIN16)/win.res $(GENDIR_WIN16)/resext.h
	$(MD) $(dir $@)
	$(CC_WIN16) $(CFLAGS_WIN16) -fo=$@ $(<:%.c=%)

#$(DEPDIR_WIN16)/%.d: %.c $(GENDIR_WIN16)/resext.h
#	$(MD) $(dir $@)
#	$(HOST_CC) -DPLATFORM_WIN16 -MM $< \
#      -MT $(subst .c,.o,$(addprefix $(DEPDIR_WIN16)/,$<)) -MF $@ || touch $@

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
   unilayer/memory/winm.c \
   unilayer/graphics/wing.c

ifeq ($(RESOURCE),DEFAULT)
   DSEKAI_C_FILES_WIN32_ONLY += unilayer/resource/winr.c
else
   DSEKAI_C_FILES_WIN32_ONLY += $(DSEKAI_C_FILES_RES)
endif

DSEKAI_O_FILES_WIN32 := \
   $(addprefix $(OBJDIR_WIN32)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_WIN32)/,$(subst .c,.o,$(DSEKAI_C_FILES_WIN32_ONLY)))

WIN32_RES_FILES := src/winstat.rc
ifeq ($(RESOURCE),DEFAULT)
   WIN32_RES_FILES += $(GENDIR_WIN32)/win.rc
endif
WIN32_RES_FILES += $(ASSETDIR)/$(DSEKAI).ico

WIN32_ASSETS :=

ifeq ($(RESOURCE),FILE)
   WIN32_ASSETS += $(DSEKAI_ASSETS_BITMAPS_16x16x4)
endif

ifeq ($(FMT_ASN),TRUE)
   WIN32_ASSETS += $(DSEKAI_ASSETS_MAPS_ASN) 
endif

ifeq ($(FMT_JSON),TRUE)
   WIN32_ASSETS += \
      $(DSEKAI_ASSETS_MAPS_JSON) \
      $(DSEKAI_ASSETS_TILESETS_JSON)
endif

# 3. Programs

CC_WIN32 := wcc386
LD_WIN32 := wcl386
RC_WIN32 := wrc

# 4. Arguments

CFLAGS_WIN32 := -bt=nt -3 -i=$(INCLUDE) -i=$(INCLUDE)/nt -DSCREEN_SCALE=2 -DPLATFORM_WIN32 $(CFLAGS_OWC_GENERIC) -zp=1 -DSCREEN_W=160 -DSCREEN_H=160 -DUSE_SOFTWARE_TEXT -i=unilayer $(DEFINES_DSEKAI) -I$(GENDIR_WIN32)

ifneq ($(RESOURCE),DEFAULT)
   CFLAGS_WIN32 += $(DEFINES_RESOURCE)
endif

LDFLAGS_WIN32 := -bcl=nt_win -zp=1 $(LDFLAGS_OWC_GENERIC)

RCFLAGS_WIN32 := -r -DPLATFORM_WIN32 -i $(INCLUDE)win

ifeq ($(RESOURCE),DEFAULT)
   RCFLAGS_WIN32 += -DRESOURCE_WIN
endif

# 5. Targets

platform := win32
res_gfx := $(DSEKAI_ASSETS_BITMAPS_16x16x4)
res_maps := $(DSEKAI_ASSETS_MAPS_JSON)
$(eval $(RESEXT_H_RULE))

pkg_bin := $(BIN_WIN32)
pkg_strip := echo
pkg_name := $(DSEKAI)-$(platform)-$(GIT_HASH)
pkg_reqs := $(WIN32_ASSETS)
$(eval $(PKG_RULE))

$(BIN_WIN32): \
$(DSEKAI_O_FILES_WIN32) $(OBJDIR_WIN32)/win.res | $(BINDIR)/$(STAMPFILE) $(addprefix $(BINDIR)/,$(WIN32_ASSETS))
	$(LD_WIN32) $(LDFLAGS_WIN32) -fe=$@ $^

$(OBJDIR_WIN32)/win.res: $(WIN32_RES_FILES) | $(OBJDIR_WIN32)/$(STAMPFILE)
	$(RC_WIN32) $(RCFLAGS_WIN32) $< -o $@

$(OBJDIR_WIN32)/%.o: %.c $(OBJDIR_WIN32)/win.res $(GENDIR_WIN32)/resext.h
	$(MD) $(dir $@)
	$(CC_WIN32) $(CFLAGS_WIN32) -fo=$@ $(<:%.c=%)

#$(DEPDIR_WIN32)/%.d: %.c $(GENDIR_WIN32)/resext.h
#	$(MD) $(dir $@)
#	$(HOST_CC) -DPLATFORM_WIN32 -MM $< \
#      -MT $(subst .c,.o,$(addprefix $(DEPDIR_WIN32)/,$<)) -MF $@ || touch $@

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
   $(subst .json,.h,$(subst $(ASSETDIR)/,$(GENDIR_MAC6)/,$(DSEKAI_ASSETS_MAPS_JSON)))

DSEKAI_O_FILES_MAC6 := \
   $(addprefix $(OBJDIR_MAC6)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_MAC6)/,$(subst .c,.o,$(DSEKAI_C_FILES_MAC6_ONLY))) \
   $(addprefix $(OBJDIR_MAC6)/,$(subst .c,.o,$(DSEKAI_C_FILES_RES)))

# 3. Programs

CC_MAC6 := m68k-apple-macos-gcc
CXX_MAC6 := m68k-apple-macos-g++
REZ_MAC6 := Rez

# 4. Arguments

CFLAGS_MAC6 := -DPLATFORM_MAC6 -I$(RETRO68_PREFIX)/multiversal/CIncludes $(CFLAGS_GCC_GENERIC) -DUSE_SOFTWARE_TEXT $(DEFINES_RESOURCE) $(DEFINES_DSEKAI) -I$(GENDIR_MAC6) -DSCREEN_W=640 -DSCREEN_H=480

LDFLAGS_MAC6 := -lRetroConsole $(LDFLAGS_GCC_GENERIC)

# 5. Targets

#$(BINDIR)/mac6.drc \
#$(GENDIR_MAC6)/resext.h: $(DSEKAI_ASSETS_PICTS) $(DRCPACK) | \
#$(GENDIR_MAC6)/$(STAMPFILE)
#	$(DRCPACK) -c -a -af $(BINDIR)/mac6.drc -t PICT -i 5001 \
#      -if $(DSEKAI_ASSETS_PICTS) $(DSEKAI_ASSETS_MAPS) \
#      -lh $(GENDIR_MAC6)/resext.h

platform := mac6
res_gfx := $(DSEKAI_ASSETS_BITMAPS_16x16x4)
res_maps := $(DSEKAI_ASSETS_MAPS_JSON)
$(eval $(RESEXT_H_RULE))

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
   $(subst .json,.h,$(subst $(ASSETDIR)/,$(GENDIR_NDS)/,$(DSEKAI_ASSETS_MAPS_JSON)))

DSEKAI_O_FILES_NDS := \
   $(addprefix $(OBJDIR_NDS)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_NDS)/,$(subst .c,.o,$(DSEKAI_C_FILES_NDS_ONLY))) \
   $(addprefix $(OBJDIR_NDS)/,$(subst .c,.o,$(DSEKAI_C_FILES_RES)))

# 3. Programs

CC_NDS := arm-none-eabi-gcc
LD_NDS := arm-none-eabi-gcc
NDSTOOL := ndstool

# 4. Arguments

ARCH_NDS := -mthumb -mthumb-interwork

CFLAGS_NDS := --sysroot $(DEVKITARM)/arm-none-eabi -I$(DEVKITPRO)/libnds/include -DPLATFORM_NDS -DARM9 -g -march=armv5te -mtune=arm946e-s -fomit-frame-pointer -ffast-math $(ARCH_NDS) -DUSE_SOFTWARE_TEXT $(DEFINES_RESOURCE) $(DEFINES_DSEKAI) $(CFLAGS_GCC_GENERIC)

LIBS_NDS := -L$(DEVKITPRO)/libnds/lib -lnds9

LDFLAGS_NDS := -specs=ds_arm9.specs -g $(ARCH_NDS) -Wl,-Map,$(OBJDIR_NDS)/$(DSEKAI).map $(LDFLAGS_GCC_GENERIC)

$(BIN_NDS): PATH := $(DEVKITPATH)/tools/bin:$(DEVKITPATH)/devkitARM/bin:$(PATH)

# 5. Targets

platform := nds
res_gfx := $(DSEKAI_ASSETS_BITMAPS_16x16x4)
res_maps := $(DSEKAI_ASSETS_MAPS)
$(eval $(RESEXT_H_RULE))

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

#$(DEPDIR_NDS)/%.d: %.c $(GENDIR_NDS)/resext.h $(DSEKAI_ASSETS_MAPS_NDS)
#	$(MD) $(dir $@)
#	$(CC_NDS) $(CFLAGS_NDS) -MM $< \
#      -MT $(subst .c,.o,$(addprefix $(DEPDIR_NDS)/,$<)) -MF $@ || touch $@

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
   unilayer/memory/fakem.c

DSEKAI_O_FILES_WEB := \
   $(addprefix $(OBJDIR_WEB)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_WEB)/,$(subst .c,.o,$(DSEKAI_C_FILES_WEB_ONLY))) \
   $(addprefix $(OBJDIR_WEB)/,$(subst .c,.o,$(DSEKAI_C_FILES_RES)))

# 3. Programs

CC_WEB := emcc
LD_WEB := emcc

# 4. Arguments

CFLAGS_WEB := -DSCREEN_SCALE=3 -DSCREEN_W=160 -DSCREEN_H=160 -std=c89 -DPLATFORM_WEB -DUSE_SOFTWARE_TEXT $(DEFINES_RESOURCE) $(DEFINES_DSEKAI) $(CFLAGS_GCC_GENERIC)

LDFLAGS_WEB := $(LDFLAGS_GCC_GENERIC)

# 5. Targets

$(GENDIR_WEB)/resext.h: \
$(DSEKAI_ASSETS_BITMAPS_16x16x4) $(DSEKAI_ASSETS_MAPS_JSON) | \
$(GENDIR_WEB)/$(STAMPFILE) $(HEADPACK)
	$(HEADPACK) $@ $^

$(BIN_WEB): $(DSEKAI_O_FILES_WEB) | $(BINDIR)/$(STAMPFILE)
	$(LD_WEB) -o $@ $^ $(LDFLAGS_WEB)

$(OBJDIR_WEB)/%.o: %.c $(GENDIR_WEB)/resext.h | $(DSEKAI_ASSETS_MAPS_JSON)
	$(MD) $(dir $@)
	$(CC_WEB) $(CFLAGS_WEB) -c -o $@ $(<:%.o=%)

#$(DEPDIR_WEB)/%.d: %.c $(GENDIR_WEB)/resext.h
#	$(MD) $(dir $@)
#	$(CC_WEB) $(CFLAGS_WEB) -MM $< \
#      -MT $(subst .c,.o,$(addprefix $(DEPDIR_WEB)/,$<)) -MF $@

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
   $(addprefix $(OBJDIR_CURSES)/,$(subst .c,.o,$(DSEKAI_C_FILES_CURSES_ONLY))) \
   $(addprefix $(OBJDIR_CURSES)/,$(subst .c,.o,$(DSEKAI_C_FILES_RES)))

# 3. Programs

CC_CURSES := gcc
LD_CURSES := gcc

# 4. Arguments

CFLAGS_CURSES := $(shell pkg-config ncurses --cflags) -g -DSCREEN_W=160 -DSCREEN_H=160 -std=c89 -DPLATFORM_CURSES $(CFLAGS_GCC_GENERIC) $(DEFINES_RESOURCE) $(DEFINES_DSEKAI)

LDFLAGS_CURSES := $(shell pkg-config ncurses --libs) $(LDFLAGS_GCC_GENERIC)

# 5. Targets

platform := curses
res_gfx := $(DSEKAI_ASSETS_BITMAPS_16x16x4)
res_maps := $(DSEKAI_ASSETS_MAPS_JSON)
$(eval $(RESEXT_H_RULE))

$(BIN_CURSES): $(DSEKAI_O_FILES_CURSES) | $(BINDIR)/$(STAMPFILE)
	$(LD_CURSES) -o $@ $^ $(LDFLAGS_CURSES)

$(OBJDIR_CURSES)/%.o: %.c $(GENDIR_CURSES)/resext.h | $(DSEKAI_ASSETS_MAPS_JSON)
	$(MD) $(dir $@)
	$(CC_CURSES) $(CFLAGS_CURSES) -c -o $@ $(<:%.o=%)

#$(DEPDIR_CURSES)/%.d: %.c $(GENDIR_CURSES)/resext.h
#	$(MD) $(dir $@)
#	$(CC_CURSES) $(CFLAGS_CURSES) -MM $< \
#      -MT $(subst .c,.o,$(addprefix $(DEPDIR_CURSES)/,$<)) -MF $@

#include $(subst $(OBJDIR)/,$(DEPDIR)/,$(DSEKAI_O_FILES_CURSES:.o=.d))

# ====== Main: SDLarm ======

# 1. Directories

OBJDIR_SDL_ARM := $(OBJDIR)/sdlarm
DEPDIR_SDL_ARM := $(DEPDIR)/sdlarm
GENDIR_SDL_ARM := $(GENDIR)/sdlarm

DSEKAI_O_FILES_SDL_ARM := \
   $(addprefix $(OBJDIR_SDL_ARM)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_SDL_ARM)/,$(subst .c,.o,$(DSEKAI_C_FILES_SDL_ONLY))) \
   $(addprefix $(OBJDIR_SDL_ARM)/,$(subst .c,.o,$(DSEKAI_C_FILES_RES)))

# 3. Programs

CC_SDL_ARM := arm-linux-gnueabihf-gcc
LD_SDL_ARM := arm-linux-gnueabihf-gcc

# 4. Arguments

CFLAGS_SDL_ARM := -I $(GENDIR_SDL) -DSCREEN_SCALE=3 $(shell pkg-config sdl2 --cflags) -g -DSCREEN_W=160 -DSCREEN_H=160 -std=c89 -DPLATFORM_SDL $(CFLAGS_GCC_GENERIC) -DUSE_SOFTWARE_TEXT $(DEFINES_RESOURCE) $(DEFINES_DSEKAI) $(FLAGS_GCC_SANITIZE)

LDFLAGS_SDL_ARM := $(shell pkg-config sdl2 --libs) $(LDFLAGS_GCC_GENERIC) $(FLAGS_GCC_SANITIZE)

# 5. Targets

platform := sdlarm
res_gfx := $(DSEKAI_ASSETS_BITMAPS_16x16x4)
res_maps := $(DSEKAI_ASSETS_MAPS_JSON)
$(eval $(RESEXT_H_RULE))

$(BIN_SDL_ARM): $(DSEKAI_O_FILES_SDL_ARM) | $(BINDIR)/$(STAMPFILE)
	$(LD_SDL_ARM) -o $@ $^ $(LDFLAGS_SDL_ARM)

$(OBJDIR_SDL_ARM)/%.o: %.c $(GENDIR_SDL_ARM)/resext.h | $(DSEKAI_ASSETS_MAPS_JSON)
	$(MD) $(dir $@)
	$(CC_SDL_ARM) $(CFLAGS_SDL_ARM) -c -o $@ $(<:%.o=%)

# ====== Main: Sega Genesis/MegaDrive ======

# 1. Directories

OBJDIR_MEGAD := $(OBJDIR)/megad
DEPDIR_MEGAD := $(DEPDIR)/megad
GENDIR_MEGAD := $(GENDIR)/megad

# 2. Files

DSEKAI_C_FILES_MEGAD_ONLY := \
   src/main.c \
   unilayer/input/megadi.c \
   unilayer/memory/fakem.c \
   unilayer/resource/megadr.c \
   unilayer/graphics/megadg.c

DSEKAI_O_FILES_MEGAD := \
   $(addprefix $(OBJDIR_MEGAD)/,$(subst .c,.o,$(DSEKAI_C_FILES))) \
   $(addprefix $(OBJDIR_MEGAD)/,$(subst .c,.o,$(DSEKAI_C_FILES_MEGAD_ONLY)))

# 3. Programs

CC_MEGAD := m68k-elf-gcc
LD_MEGAD := m68k-elf-gcc

# 4. Arguments

CFLAGS_MEGAD := -m68000 -fno-builtin -I/opt/gendev/sgdk/inc -I/opt/gendev/sgdk/res -B/opt/gendev/sgdk/bin $(CFLAGS_GCC_GENERIC) -I$(GENDIR_MEGAD) -DPLATFORM_MEGADRIVE -DSCREEN_W=320 -DSCREEN_H=224 -Os

LDFLAGS_MEGAD := $(CFLAGS_PALM)

# 5. Targets

platform := megad
res_gfx := $(DSEKAI_ASSETS_BITMAPS_16x16x4)
res_maps := $(DSEKAI_ASSETS_MAPS_JSON)
$(eval $(RESEXT_H_RULE))

pkg_bin := $(BIN_MEGAD)
pkg_strip := echo
pkg_name := $(DSEKAI)-$(platform)-$(GIT_HASH)
pkg_reqs :=
$(eval $(PKG_RULE))

$(BIN_MEGAD): $(DSEKAI_O_FILES_MEGAD) | $(BINDIR)/$(STAMPFILE)
	$(LD_MEGAD) -o $@ $^ $(LDFLAGS_MEGAD)

$(OBJDIR_MEGAD)/%.o: %.c $(GENDIR_MEGAD)/resext.h
	$(MD) $(dir $@)
	$(CC_MEGAD) $(CFLAGS_MEGAD) -c -o $@ $(<:%.o=%)

# ====== Check: Null ======

$(GENDIR_CHECK_NULL)/resext.h: $(GENDIR_CHECK_NULL)/$(STAMPFILE) $(MKRESH)
	$(MKRESH) -f palm -i 5001 \
      -if $(DSEKAI_ASSETS_PALM) $(DSEKAI_ASSETS_MAPS_JSON) \
      -oh $(GENDIR_CHECK_NULL)/resext.h

$(BIN_CHECK_NULL): $(DSEKAI_O_FILES_CHECK_NULL) | $(BINDIR)/$(STAMPFILE)
	$(CC) -o $@ $^ $(LDFLAGS)

#$(DEPDIR_CHECK_NULL)/%.d: %.c $(GENDIR_CHECK_NULL)/resext.h
#	$(MD) $(dir $@)
#	$(CC) $(CFLAGS_CHECK_NULL) -MM $< \
#      -MT $(subst .c,.o,$(addprefix $(DEPDIR_CHECK_NULL)/,$<)) -MF $@

#include $(subst $(OBJDIR)/,$(DEPDIR)/,$(DSEKAI_O_FILES_CHECK_NULL:.o=.d))
	
$(OBJDIR_CHECK_NULL)/%.o: %.c check/testdata.h $(GENDIR_CHECK_NULL)/resext.h
	$(MD) $(dir $@)
	$(CC) $(CFLAGS_CHECK_NULL) -c -o $@ $(<:%.o=%)

# ====== Clean ======

clean:
	rm -rf data obj obj-file bin bin-file gen gen-file *.err .rsrc .finf gmon.out log*.txt packages fpackages assets/*/*.cga assets/*.asn

