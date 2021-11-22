
# vi:syntax=make

DSEKAI := dsekai

ENTRY_MAP := field

ROOT := $(shell pwd)

DSEKAI_C_FILES := \
   src/tilemap.c \
   unilayer/src/graphics.c \
   unilayer/src/animate.c \
   src/mobile.c \
   src/item.c \
   src/window.c \
   src/script.c \
   unilayer/src/dio.c \
   src/control.c \
   src/topdown.c \
   src/pov.c \
   src/title.c

MKRESH_C_FILES := \
   tools/mkresh.c \
   unilayer/src/resource/file.c \
   unilayer/src/memory/fakem.c \
   unilayer/src/dio.c

HEADPACK_C_FILES := \
   tools/headpack.c \
   src/tmjson.c \
   src/tilemap.c \
   src/mobile.c \
   src/script.c \
   src/json.c \
   unilayer/src/resource/file.c \
   unilayer/src/graphics.c \
   unilayer/src/graphics/nullg.c \
   unilayer/src/memory/fakem.c \
   unilayer/src/dio.c

CONVERT_C_FILES := \
   unilayer/tools/convert.c \
   unilayer/tools/data/bmp.c \
   unilayer/src/memory/fakem.c \
   unilayer/tools/data/cga.c \
   unilayer/src/dio.c \
   unilayer/tools/data/icns.c \
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
   src/asn.c \
   unilayer/src/resource/file.c \
   unilayer/src/graphics.c \
   unilayer/src/graphics/nullg.c \
   unilayer/src/memory/fakem.c \
   unilayer/src/dio.c

MAP2BIN_C_FILES := \
   tools/map2bin.c \
   src/tilemap.c \
   src/mobile.c \
   src/script.c \
   src/json.c \
   src/tmjson.c \
   unilayer/src/resource/file.c \
   unilayer/src/graphics.c \
   unilayer/src/graphics/nullg.c \
   unilayer/src/memory/fakem.c \
   unilayer/src/dio.c

# ALL platforms.
PLATFORMS := sdl xlib dos win16 win32 palm mac6 nds curses check_null

# Platforms on which RESOURCE=FILE is working.
PLATFORMS_FILE := sdl xlib win32

ASSETDIR := assets
ASSETPATH :=
DTHRESHOLD := 2

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
   DEFINES_DEPTH := -DDEPTH_VGA -DDEPTH_SPEC=\"16x16x16\"
else ifeq ($(DEPTH),MONO)
   DEPTH_SPEC := 16x16x2
   PKG_OUT_FLAGS := $(PKG_OUT_FLAGS)-mono
   BINDIR := $(BINDIR)-mono
   GENDIR := $(GENDIR)-mono
   DEPDIR := $(DEPDIR)-mono
   OBJDIR := $(OBJDIR)-mono
   DEFINES_DEPTH := -DDEPTH_MONO -DDEPTH_SPEC=\"16x16x2\"
else
   DEPTH := CGA
   DEPTH_SPEC := 16x16x4
   PKG_OUT_FLAGS := $(PKG_OUT_FLAGS)-cga
   DEFINES_DEPTH := -DDEPTH_CGA -DDEPTH_SPEC=\"16x16x4\"
endif

ifeq ($(FMT_ASN),TRUE)
   BINDIR := $(BINDIR)-asn
   GENDIR := $(GENDIR)-asn
   DEPDIR := $(DEPDIR)-asn
   OBJDIR := $(OBJDIR)-asn
endif

ifeq ($(FMT_JSON),TRUE)
   BINDIR := $(BINDIR)-json
   GENDIR := $(GENDIR)-json
   DEPDIR := $(DEPDIR)-json
   OBJDIR := $(OBJDIR)-json
endif

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
BIN_CHECK := $(BINDIR)/check

PKGDIR := packages
PKGOS := $(shell uname -s -m | sed 's/ /-/g' | tr '[:upper:]' '[:lower:]'])

GIT_HASH := $(shell git log -1 --pretty=format:"%h")

DEFINES_DSEKAI := -DUNILAYER_PROJECT_NAME=\"dsekai\" -DDSEKAI_GIT_HASH=\"$(GIT_HASH)\" -DENTRY_MAP=$(ENTRY_MAP)

ifeq ($(RESOURCE),FILE)

   DEFINES_RESOURCE := -DRESOURCE_FILE -DASSETS_PATH="\"$(ASSETPATH)\""
   DSEKAI_C_FILES_RES := unilayer/src/resource/file.c src/json.c src/asn.c
   PKG_OUT_FLAGS := $(PKG_OUT_FLAGS)-file

   ifeq ($(FMT_ASN),TRUE)

all: $(BIN_DOS) $(BIN_SDL) $(BIN_XLIB) $(BIN_WIN32) $(BIN_WIN16)

   else

all: $(BIN_SDL) $(BIN_XLIB) $(BIN_WIN32)

   endif

else

   RESOURCE := DEFAULT
   DEFINES_RESOURCE := -DRESOURCE_HEADER
   DSEKAI_C_FILES_RES := unilayer/src/resource/header.c

all: $(BIN_DOS) $(BIN_SDL) $(BIN_XLIB) $(BIN_WIN16) $(BIN_WIN32)

endif

CFLAGS_GCC_GENERIC := --std=c89
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

ifeq ($(SCREEN_SCALE),)
   SCREEN_SCALE := 3
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
   $(wildcard $(ASSETDIR)/$(DEPTH_SPEC)/p_*.bmp) \
   $(wildcard $(ASSETDIR)/$(DEPTH_SPEC)/title.bmp)

DSEKAI_ASSETS_MAPS_JSON := \
   $(ASSETDIR)/m_field.json \
   $(ASSETDIR)/m_templ.json \
   $(ASSETDIR)/m_forst.json

DSEKAI_ASSETS_TILESETS_JSON := \
   $(ASSETDIR)/t2_field.json \
   $(ASSETDIR)/t2_templ.json \
   $(ASSETDIR)/t2_forst.json

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
CONVERT := $(BINDIR)/convert
LOOKUPS := $(BINDIR)/lookups
HEADPACK := $(BINDIR)/headpack
MAP2ASN := $(BINDIR)/map2asn

CFLAGS_MKRESH := -DNO_RESEXT -g -DDEBUG_LOG -DDEBUG_THRESHOLD=0 -DRESOURCE_FILE -Iunilayer/src -DASSETS_PATH="\"$(ASSETPATH)\""
CFLAGS_CONVERT := -DNO_RESEXT -g -DRESOURCE_FILE -Iunilayer/src
CFLAGS_LOOKUPS := -g -Iunilayer/src
CFLAGS_HEADPACK := -g -Iunilayer/src -DNO_RESEXT -DDEBUG_THRESHOLD=3 -DRESOURCE_FILE -DASSETS_PATH="\"$(ASSETPATH)\"" -DDEBUG_LOG
CFLAGS_MAP2ASN := -g -Iunilayer/src -DNO_RESEXT -DDEBUG_THRESHOLD=1 -DRESOURCE_FILE -DASSETS_PATH="\"$(ASSETPATH)\"" -DDEBUG_LOG $(FLAGS_GCC_SANITIZE)

$(BIN_CHECK): LDFLAGS := $(shell pkg-config check --libs) -g $(LDFLAGS_GCC_GENERIC)

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

$(BINDIR)/$(ASSETDIR)/%.cga: \
$(ASSETDIR)/%.cga | $(BINDIR)/$(ASSETDIR)/$(STAMPFILE)
	$(MD) $(dir $@)
	cp $^ $@

$(BINDIR)/$(ASSETDIR)/%.vga: \
$(ASSETDIR)/%.vga | $(BINDIR)/$(ASSETDIR)/$(STAMPFILE)
	$(MD) $(dir $@)
	cp $^ $@

$(BINDIR)/$(ASSETDIR)/%.json: \
$(ASSETDIR)/%.json | $(BINDIR)/$(ASSETDIR)/$(STAMPFILE)
	$(MD) $(dir $@)
	cp $^ $@

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

   $(GENDIR)/$(platform)/%.cga: %.cga | \
   $(GENDIR)/$(platform)/$(STAMPFILE)
		$(MD) $$(dir $$@)
		cp $$< $$@

   $(GENDIR)/$(platform)/%.vga: %.vga | \
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
   $(GENDIR)/$(platform)/$(ASSETDIR)/m_%.json: \
   $(ASSETDIR)/m_%.json $(GENDIR)/$(platform)/$(ASSETDIR)/t2_%.json \
   | $(GENDIR)/$(platform)/$(STAMPFILE)
		$(MD) $$(dir $$@)
		cat $$< | sed -e 's/16x16x4/$(DEPTH_SPEC)/g' > $$@

   # Generate ASN from JSON.
   $(GENDIR)/$(platform)/$(ASSETDIR)/m_%.asn: \
   $(GENDIR)/$(platform)/$(ASSETDIR)/m_%.json \
   $(GENDIR)/$(platform)/$(ASSETDIR)/t2_%.json \
   | $(MAP2ASN)
		cd $(GENDIR)/$(platform) && ../../$(MAP2ASN) \
         $$(subst $(GENDIR)/$(platform)/,,$$<) ../../$$@

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

ifeq ($(BUILD),RELEASE)
   $$(PKGBUILD)/$$(notdir $(pkg_bin)): $(pkg_bin)
		$(MD) $$(dir $$@)
		cp $$^ $$@
		$(pkg_strip) $$@
else
   $$(PKGBUILD)/$$(notdir $(pkg_bin)): $(pkg_bin)
		$(MD) $$(dir $$@)
		cp $$^ $$@
endif

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

$(CONVERT): $(CONVERT_C_FILES) | $(BINDIR)/$(STAMPFILE)
	$(HOST_CC) $(CFLAGS_CONVERT) -o $@ $^

$(LOOKUPS): $(LOOKUPS_C_FILES) | $(BINDIR)/$(STAMPFILE)
	$(HOST_CC) $(CFLAGS_LOOKUPS) -o $@ $^

$(HEADPACK): $(HEADPACK_C_FILES) | $(BINDIR)/$(STAMPFILE)
	$(HOST_CC) $(CFLAGS_HEADPACK) -o $@ $^

$(MAP2ASN): $(MAP2ASN_C_FILES) | $(BINDIR)/$(STAMPFILE)
	$(HOST_CC) $(CFLAGS_MAP2ASN) -o $@ $^

# ====== Clean ======

clean:
	rm -rf data obj obj-* bin bin-* gen gen-* dep dep-* *.err .rsrc .finf gmon.out log*.txt packages pkgbuild

