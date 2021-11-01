
# vi:syntax=make

DSEKAI := dsekai

ENTRY_MAP := field

ROOT := $(shell pwd)

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

PKG_OUT_FLAGS :=

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

ifeq ($(DEPTH),VGA)
   DEPTH_SPEC := 16x16x16
   PKG_OUT_FLAGS := $(PKG_OUT_FLAGS)-vga
   BINDIR := $(BINDIR)-vga
   GENDIR := $(GENDIR)-vga
   DEPDIR := $(DEPDIR)-vga
   OBJDIR := $(OBJDIR)-vga
   DEFINES_DEPTH := -DDEPTH_VGA
else ifeq ($(DEPTH),MONO)
   DEPTH_SPEC := 16x16x2
   PKG_OUT_FLAGS := $(PKG_OUT_FLAGS)-mono
   BINDIR := $(BINDIR)-mono
   GENDIR := $(GENDIR)-mono
   DEPDIR := $(DEPDIR)-mono
   DEFINES_DEPTH := -DDEPTH_MONO
else
   DEPTH := CGA
   DEPTH_SPEC := 16x16x4
   PKG_OUT_FLAGS := $(PKG_OUT_FLAGS)-cga
   DEFINES_DEPTH := -DDEPTH_CGA
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

DEFINES_DSEKAI := -DUNILAYER_PROJECT_NAME=\"dsekai\" -DDSEKAI_GIT_HASH=\"$(GIT_HASH)\" -DENTRY_MAP=$(ENTRY_MAP)

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

DSEKAI_ASSETS_SPRITES := \
   $(wildcard $(ASSETDIR)/$(DEPTH_SPEC)/s_*.bmp)
DSEKAI_ASSETS_TILES := \
   $(wildcard $(ASSETDIR)/$(DEPTH_SPEC)/t_*.bmp)
DSEKAI_ASSETS_PATTERNS := \
   $(wildcard $(ASSETDIR)/$(DEPTH_SPEC)/p_*.bmp)
DSEKAI_ASSETS_BITMAPS := \
   $(wildcard $(ASSETDIR)/$(DEPTH_SPEC)/s_*.bmp) \
   $(wildcard $(ASSETDIR)/$(DEPTH_SPEC)/t_*.bmp) \
   $(wildcard $(ASSETDIR)/$(DEPTH_SPEC)/p_*.bmp)

DSEKAI_ASSETS_MAPS_JSON := \
   $(ASSETDIR)/m_field.json

DSEKAI_ASSETS_TILESETS_JSON := \
   $(ASSETDIR)/t2_field.json

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

pkgbuild/$(STAMPFILE):
	$(MD) $(dir $@)
	touch $@

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

   # Copy miscellaneous files into gendir to funnel to packager.
   $(GENDIR)/$(platform)/%.bmp: %.bmp | \
   $(GENDIR)/$(platform)/$(STAMPFILE)
		$(MD) $$(dir $$@)
		cp $$< $$@

   $(GENDIR)/$(platform)/%.md: %.md | \
   $(GENDIR)/$(platform)/$(STAMPFILE)
		$(MD) $$(dir $$@)
		cp $$< $$@

   # Preprocess JSON maps/tilesets for depth path.
   $(GENDIR)/$(platform)/$(ASSETDIR)/t2_%.json: $(ASSETDIR)/t2_%.json \
   | $(GENDIR)/$(platform)/$(STAMPFILE)
		$(MD) $$(dir $$@)
		cat $$< | sed -e 's/16x16x4/$(DEPTH_SPEC)/g' > $$@

   # Require tileset for map.
   $(GENDIR)/$(platform)/$(ASSETDIR)/m_%.json: $(ASSETDIR)/m_%.json $(GENDIR)/$(platform)/$(ASSETDIR)/t2_%.json \
   | $(GENDIR)/$(platform)/$(STAMPFILE)
		$(MD) $$(dir $$@)
		cat $$< | sed -e 's/16x16x4/$(DEPTH_SPEC)/g' > $$@

   # Generate ASN from JSON.
   $(GENDIR)/$(platform)/%.asn: $(GENDIR)/$(platform)/%.json | $(MAP2ASN)
		$(MAP2ASN) $< $@

   res_maps := $(addprefix $(GENDIR)/$(platform)/,$(DSEKAI_ASSETS_MAPS_JSON))

   ifeq ($$(RESOURCE),HEADER)

      # Header was specified explicitly.

      $(GENDIR)/$(platform)/resemb.h: $(res_gfx) $$(res_maps) | \
      $(GENDIR)/$(platform)/$(STAMPFILE) $(HEADPACK)
			$(HEADPACK) $$@ $$^

      $$(RESEXT_H): $(GENDIR)/$(platform)/resemb.h
			echo '#include "resemb.h"' > $$@

   else ifeq ($(RESOURCE),FILE)

      # File was specified explicitly.

      #$(GENDIR)/$(platform)/residx.h: $(res_gfx) $$(res_maps) | \
      #$(GENDIR)/$(platform)/$(STAMPFILE) $(MKRESH)
		#	$(MKRESH) -f file -s bmp jsn json cga vga -if $$^ -oh $$@

      $$(RESEXT_H): \
      $(GENDIR)/$(platform)/$(STAMPFILE) $(res_gfx) $$(res_maps)
			#echo '#include "residx.h"' > $$@
			touch $$@

   else ifeq ($(RESOURCE)$(findstring win,$(platform)),DEFAULTwin)

      # For Windows, default to Windows resources for images.

      $(GENDIR)/$(platform)/win.rc \
      $(GENDIR)/$(platform)/residx.h: $(res_gfx) | \
      $(MKRESH) $(GENDIR)/$(platform)/$(STAMPFILE)
			$(MKRESH) -f win16 -i 5001 -s bmp cga vga \
            -if $$^ \
            -oh $(GENDIR)/$(platform)/residx.h \
            -or $(GENDIR)/$(platform)/win.rc

      # Embed maps in preparsed structs.

      $(GENDIR)/$(platform)/resemb.h: $$(res_maps) | \
      $(GENDIR)/$(platform)/$(STAMPFILE) $(HEADPACK)
			$(HEADPACK) $$@ $$^

      $$(RESEXT_H): \
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

      $(GENDIR)/$(platform)/resemb.h: $$(res_maps) | \
      $(GENDIR)/$(platform)/$(STAMPFILE) $(HEADPACK)
			$(HEADPACK) $$@ $$^

      $$(RESEXT_H): \
      $(GENDIR)/$(platform)/resemb.h $(GENDIR)/$(platform)/residx.h
			echo '#include "residx.h"' > $$@
			echo '#include "resemb.h"' >> $$@

   else

      # For all other platforms, default to header.

      $(GENDIR)/$(platform)/resemb.h: $(res_gfx) $$(res_maps) | \
      $(GENDIR)/$(platform)/$(STAMPFILE) $(HEADPACK)
			$(HEADPACK) $$@ $$^

      $$(RESEXT_H): $(GENDIR)/$(platform)/resemb.h
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
   PKGBUILD := pkgbuild/$(pkg_name)$(PKG_OUT_FLAGS)

   $$(PKGBUILD)/%: $(GENDIR)/$(platform)/%
		$(MD) $$(dir $$@)
		cp $$< $$@

   $$(PKGBUILD)/$$(notdir $(pkg_bin)): $(pkg_bin)
		$(MD) $$(dir $$@)
		cp $$^ $$@
		$(pkg_strip) $$@

   $(ROOT)/$(PKGDIR)/$(pkg_name)$(PKG_OUT_FLAGS).tar.gz: \
   $$(addprefix $$(PKGBUILD)/,$(pkg_reqs)) \
   $$(PKGBUILD)/$(notdir $(pkg_bin)) \
   $$(PKGBUILD)/README.md \
   | $(PKGDIR)/$(STAMPFILE)
		cd pkgbuild && $(TAR) -cvf - $(pkg_name)$(PKG_OUT_FLAGS) | $(GZIP) > $$@

   $(ROOT)/$(PKGDIR)/$(pkg_name)$(PKG_OUT_FLAGS).zip: \
   $$(addprefix $$(PKGBUILD)/,$(pkg_reqs)) \
   $$(PKGBUILD)/$(notdir $(pkg_bin)) \
   $$(PKGBUILD)/README.md \
   | $(PKGDIR)/$(STAMPFILE)
		cd pkgbuild && $(ZIP) -r $$@ $(pkg_name)$(PKG_OUT_FLAGS)

   pkg_$(platform): $(ROOT)/$(PKGDIR)/$(pkg_name)$(PKG_OUT_FLAGS)$(PKG_OUT_EXT)
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
	rm -rf data obj obj-* bin bin-* gen gen-* *.err .rsrc .finf gmon.out log*.txt packages fpackages assets/*/*.cga assets/*/*.vga assets/*.asn pkgbuild

