
# vi:syntax=make

DSEKAI := dsekai

ENTRY_MAP := field

ROOT := $(shell pwd)

STAMPFILE := .stamp

DSEKAI_C_FILES_ENGINES := \
   src/topdown.c \
   src/title.c

# Some platforms (e.g. Palm) care about common object files being close to all
# constituents that might use them, so this allows us to optionally insert them
# in the middle in the platform-specifict Makefiles.

DSEKAI_C_FILES_SUPPORT_1 := \
   src/tilemap.c \
   src/mobile.c \
   src/item.c \
   src/script.c \
   src/engines.c \
   unilayer/src/window.c \
   unilayer/src/dio.c
DSEKAI_C_FILES_SUPPORT_2 := \
   unilayer/src/graphics.c \
   unilayer/src/input.c \
   src/menu.c \
   src/crop.c \
   src/strpool.c \
   src/pathfind.c \
   src/tmasn.c \
   src/serial/asn.c \
   unilayer/src/asn.c

DSEKAI_C_FILES_SUPPORT := \
   $(DSEKAI_C_FILES_SUPPORT_1) \
   $(DSEKAI_C_FILES_SUPPORT_2)

DSEKAI_C_FILES := \
   $(DSEKAI_C_FILES_ENGINES) \
   $(DSEKAI_C_FILES_SUPPORT)

LOOKUPS_C_FILES := \
   tools/lookups.c

MAP2ASN_C_FILES := \
   tools/map2asn.c \
   src/script.c \
   src/scparse.c \
   unilayer/src/json.c \
   src/tmjson.c \
   unilayer/src/asn.c \
   src/strpool.c \
   unilayer/src/resource/file.c \
   unilayer/src/graphics.c \
   unilayer/src/graphics/nullg.c \
   unilayer/src/memory/fakem.c \
   unilayer/src/dio.c \
   unilayer/src/save/file.c \
   src/tmasn.c

COMPILE_C_FILES := \
   unilayer/src/resource/file.c \
   unilayer/src/graphics.c \
   unilayer/src/graphics/nullg.c \
   unilayer/src/memory/fakem.c \
   unilayer/src/dio.c \
   unilayer/src/save/file.c \
   tools/compile.c \
   src/script.c

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
   DEFINES_DEPTH := -DDEPTH_VGA
else ifeq ($(DEPTH),MONO)
   DEPTH_SPEC := 16x16x2
   PKG_OUT_FLAGS := $(PKG_OUT_FLAGS)-mono
   BINDIR := $(BINDIR)-mono
   GENDIR := $(GENDIR)-mono
   DEPDIR := $(DEPDIR)-mono
   OBJDIR := $(OBJDIR)-mono
   DEFINES_DEPTH := -DDEPTH_MONO
else
   DEPTH := CGA
   DEPTH_SPEC := 16x16x4
   PKG_OUT_FLAGS := $(PKG_OUT_FLAGS)-cga
   DEFINES_DEPTH := -DDEPTH_CGA
endif

ifeq ($(BUILD),RELEASE)
   BINDIR := $(BINDIR)-release
   DEPDIR := $(DEPDIR)-release
   OBJDIR := $(OBJDIR)-release
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

BIN_CHECK := $(BINDIR)/check

PKGDIR := packages
PKGOS := $(shell uname -s -m | sed 's/ /-/g' | tr '[:upper:]' '[:lower:]'])

GIT_HASH := $(shell git log -1 --pretty=format:"%h")

DEFINES_DSEKAI := -DUNILAYER_PROJECT_NAME=\"dsekai\" -DDSEKAI_GIT_HASH=\"$(GIT_HASH)\" -DENTRY_MAP=$(ENTRY_MAP)

ifeq ($(RESOURCE),FILE)

   DEFINES_RESOURCE := -DRESOURCE_FILE -DASSETS_PATH="\"$(ASSETDIR)/\""
   DSEKAI_C_FILES_RES := unilayer/src/resource/file.c
   PKG_OUT_FLAGS := $(PKG_OUT_FLAGS)-file

else

   # Specify "DEFAULT" here because some platforms (e.g. win/palm) use their
   # own resource format rather than headers.
   RESOURCE := DEFAULT
   DEFINES_RESOURCE := -DRESOURCE_HEADER
   DSEKAI_C_FILES_RES := unilayer/src/resource/header.c

endif

CFLAGS_GCC_GENERIC := --std=c89
CFLAGS_OWC_GENERIC :=

ifeq ($(BUILD),RELEASE)

   LDFLAGS_OWC_GENERIC :=
   SANITIZE := NO
   PKG_OUT_FLAGS := $(PKG_OUT_FLAGS)-release

   CFLAGS_GCC_GENERIC += -Os
   LDFLAGS_GCC_GENERIC := -Os

else

   # Assume debug build.

   CFLAGS_GCC_GENERIC += -Wall -Wno-missing-braces -Wno-char-subscripts -pg -g
   DEFINES_DSEKAI += -DDEBUG_LOG -DDEBUG_THRESHOLD=$(DTHRESHOLD)
   LDFLAGS_GCC_GENERIC := -g -pg
   PKG_OUT_FLAGS := $(PKG_OUT_FLAGS)-debug

endif

ifneq ($(FPS),)
   DEFINES_DSEKAI += -DFPS=$(FPS)
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
      DSEKAI_C_FILES_RES +=
      PKG_OUT_FLAGS := $(PKG_OUT_FLAGS)-asn
   endif

   ifeq ($(FMT_JSON),TRUE)
      DEFINES_DSEKAI += -DTILEMAP_FMT_JSON
      DSEKAI_C_FILES_RES += src/tmjson.c unilayer/src/json.c
      TILEMAP_FMTS += JSON
      PKG_OUT_FLAGS := $(PKG_OUT_FLAGS)-json
   endif

endif

ifeq ($(ARCFMT),ZIP)
   PKG_OUT_EXT := .zip
else
   PKG_OUT_EXT := .tar.gz
endif

include unilayer/Makefile

HEADPACK_C_FILES := \
   $(HEADPACK_C_FILES_BASE) \
   tools/mappack.c \
   src/tmjson.c \
   src/scparse.c \
   src/tilemap.c \
   unilayer/src/json.c \
   src/strpool.c

DSEKAI_ASSETS_SPRITES := \
   $(wildcard $(ASSETDIR)/$(DEPTH_SPEC)/s_*.bmp)
DSEKAI_ASSETS_TILES := \
   $(wildcard $(ASSETDIR)/$(DEPTH_SPEC)/t_*.bmp)
DSEKAI_ASSETS_PATTERNS := \
   $(wildcard $(ASSETDIR)/$(DEPTH_SPEC)/p_*.bmp)
DSEKAI_ASSETS_ITEMS := \
   $(wildcard $(ASSETDIR)/$(DEPTH_SPEC)/i_*.bmp)
DSEKAI_ASSETS_CROPS := \
   $(wildcard $(ASSETDIR)/$(DEPTH_SPEC)/c_*.bmp)
DSEKAI_ASSETS_BITMAPS := \
   $(DSEKAI_ASSETS_SPRITES) \
   $(DSEKAI_ASSETS_TILES) \
   $(DSEKAI_ASSETS_PATTERNS) \
   $(DSEKAI_ASSETS_ITEMS) \
   $(DSEKAI_ASSETS_CROPS)
DSEKAI_ASSETS_SCRIPTS := $(wildcard $(ASSETDIR)/*/*.s)

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

LOOKUPS := $(BINDIR)/lookups
MAP2ASN := $(BINDIR)/map2asn
COMPILE := $(BINDIR)/compile

DEFINES_MAP2ASN := -DNO_RESEXT -DDEBUG_THRESHOLD=1 -DRESOURCE_FILE -DASSETS_PATH="\"$(ASSETPATH)\"" -DDEBUG_LOG -DDISABLE_WEATHER_EFFECTS -DNO_GUI -DNO_DEBUG_LINE_NUMBER -DNO_ANIMATE -DNO_SCRIPT_HANDLERS -DNO_ITEM_HANDLERS -DUNILAYER_PROJECT_NAME=\"map2asn\" -DGIT_HASH=\"$(GIT_HASH)\" $(DEFINES_DEPTH)

DEFINES_COMPILE := -DNO_RESEXT -DDEBUG_THRESHOLD=1 -DRESOURCE_FILE -DDEBUG_LOG -DDISABLE_WEATHER_EFFECTS -DNO_GUI -DNO_DEBUG_LINE_NUMBER -DNO_ANIMATE -DNO_SCRIPT_HANDLERS -DNO_ITEM_HANDLERS -DUNILAYER_PROJECT_NAME=\"compile\" -DGIT_HASH=\"$(GIT_HASH)\"

CFLAGS_LOOKUPS := -g $(INCLUDES_UNILAYER)
CFLAGS_MAP2ASN := -g $(INCLUDES_UNILAYER) $(FLAGS_GCC_SANITIZE) $(DEFINES_MAP2ASN) -DPLATFORM_NULL
CFLAGS_COMPILE := -g $(INCLUDES_UNILAYER) $(FLAGS_GCC_SANITIZE) $(DEFINES_COMPILE) -DPLATFORM_NULL

$(BIN_CHECK): LDFLAGS := $(shell pkg-config check --libs) -g $(LDFLAGS_GCC_GENERIC)

# ====== Generic Rules ======

pkgbuild/$(STAMPFILE):
	$(MD) $(dir $@)
	touch $@

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

   $(GENDIR)/$(platform)/%.cga: %.bmp | \
   $(GENDIR)/$(platform)/$(STAMPFILE) $(CONVERT)
		$(MD) $$(dir $$@)
		$(CONVERT) -ic bmp -oc cga -ob 2 -if $$< -of $$@ -og

   $(GENDIR)/$(platform)/%.vga: %.bmp | \
   $(GENDIR)/$(platform)/$(STAMPFILE) $(CONVERT)
		$(MD) $$(dir $$@)
		# TODO: Fix the output bits here!
		$(CONVERT) -ic bmp -oc cga -ob 4 -if $$< -of $$@ -og

   $(GENDIR)/$(platform)/%.md: %.md | \
   $(GENDIR)/$(platform)/$(STAMPFILE)
		$(MD) $$(dir $$@)
		cp $$< $$@

   $(GENDIR)/$(platform)/$(ASSETDIR)/%.s: $(ASSETDIR)/%.s
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
   $(addprefix $(GENDIR)/$(platform)/,$(DSEKAI_ASSETS_SCRIPTS)) \
   | $(MAP2ASN)
		cd $(GENDIR)/$(platform) && ../../$(MAP2ASN) \
         $$(subst $(GENDIR)/$(platform)/,,$$<) ../../$$@

   res_maps := $(addprefix $(GENDIR)/$(platform)/,$(DSEKAI_ASSETS_MAPS_JSON))

   # TODO: This should be worked into the individual makefiles. See NDS.
   ifeq ($$(RESOURCE),HEADER)

      # Header was specified explicitly.

      $(GENDIR)/$(platform)/resemb.h: $(res_gfx) $$(res_maps) | \
      $(GENDIR)/$(platform)/$(STAMPFILE) $(HEADPACK) \
      $(addprefix $(GENDIR)/$(platform)/,$(DSEKAI_ASSETS_SCRIPTS))
			$(HEADPACK) $$@ $$^

      $$(RESEXT_H_$(platform_upper)): $(GENDIR)/$(platform)/resemb.h
			echo '#include "resemb.h"' > $$@

   else ifeq ($(RESOURCE),FILE)

      # File was specified explicitly.

      #$(GENDIR)/$(platform)/residx.h: $(res_gfx) $$(res_maps) | \
      #$(GENDIR)/$(platform)/$(STAMPFILE) $(MKRESH)
		#	$(MKRESH) -f file -s bmp jsn json cga vga -if $$^ -oh $$@

      $$(RESEXT_H_$(platform_upper)): \
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
      $(GENDIR)/$(platform)/$(STAMPFILE) $(HEADPACK) \
      $(addprefix $(GENDIR)/$(platform)/,$(DSEKAI_ASSETS_SCRIPTS))
			$(HEADPACK) $$@ $$^

      $$(RESEXT_H_$(platform_upper)): \
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
            -oa $(GENDIR)/$(platform)/resarr.h \
            -or $(GENDIR)/$(platform)/palmd.rcp

      # Embed maps in preparsed structs.

      $(GENDIR)/$(platform)/resemb.h: $$(res_maps) | \
      $(GENDIR)/$(platform)/$(STAMPFILE) $(HEADPACK) \
      $(addprefix $(GENDIR)/$(platform)/,$(DSEKAI_ASSETS_SCRIPTS))
			$(HEADPACK) $$@ $$^

      $$(RESEXT_H_$(platform_upper)): \
      $(GENDIR)/$(platform)/resemb.h $(GENDIR)/$(platform)/residx.h
			echo '#include "residx.h"' > $$@
			echo '#include "resemb.h"' >> $$@

   else

      # For all other platforms, default to header.

      $(GENDIR)/$(platform)/resemb.h: $(res_gfx) $$(res_maps) | \
      $(GENDIR)/$(platform)/$(STAMPFILE) $(HEADPACK) \
      $(addprefix $(GENDIR)/$(platform)/,$(DSEKAI_ASSETS_SCRIPTS))
			$(HEADPACK) $$@ $$^

      $$(RESEXT_H_$(platform_upper)): $(GENDIR)/$(platform)/resemb.h
			echo '#include "resemb.h"' > $$@

   endif

endef

define PKG_RULE
   PKGBUILD_$(platform_upper) := pkgbuild/$(pkg_name)$(PKG_OUT_FLAGS)

   $$(PKGBUILD_$(platform_upper))/convert%: $(CONVERT)%
		$(MD) $$(dir $$@)
		cp $$< $$@

   $$(PKGBUILD_$(platform_upper))/map2asn%: $(MAP2ASN)%
		$(MD) $$(dir $$@)
		cp $$< $$@

   $$(PKGBUILD_$(platform_upper))/README.md: README.md
		$(MD) $$(dir $$@)
		cp README.md $$@

   $$(PKGBUILD_$(platform_upper))/%: $(GENDIR)/$(platform)/%
		$(MD) $$(dir $$@)
		cp $$< $$@

# Copy binary (strip if release).
ifeq ($(BUILD),RELEASE)
   $$(PKGBUILD_$(platform_upper))/$$(notdir $(pkg_bin)): $(pkg_bin)
		$(MD) $$(dir $$@)
		cp $$^ $$(PKGBUILD_$(platform_upper))
		$(pkg_strip) $$@
else
   $$(PKGBUILD_$(platform_upper))/$$(notdir $(pkg_bin)): $(pkg_bin)
		$(MD) $$(dir $$@)
		cp $$^ $$(PKGBUILD_$(platform_upper))
endif

   $(ROOT)/$(PKGDIR)/$(pkg_name)$(PKG_OUT_FLAGS).tar.gz: \
   $$(addprefix $$(PKGBUILD_$(platform_upper))/,$(pkg_reqs)) \
   $$(addprefix $$(PKGBUILD_$(platform_upper))/,$(notdir $(pkg_bin))) \
   $$(PKGBUILD_$(platform_upper))/README.md \
   | $(PKGDIR)/$(STAMPFILE)
		cd pkgbuild && $(TAR) -cvf - $(pkg_name)$(PKG_OUT_FLAGS) | $(GZIP) > $$@

   $(ROOT)/$(PKGDIR)/$(pkg_name)$(PKG_OUT_FLAGS).zip: \
   $$(addprefix $$(PKGBUILD_$(platform_upper))/,$(pkg_reqs)) \
   $$(addprefix $$(PKGBUILD_$(platform_upper))/,$(notdir $(pkg_bin))) \
   $$(PKGBUILD_$(platform_upper))/README.md \
   | $(PKGDIR)/$(STAMPFILE)
		cd pkgbuild && $(ZIP) -r $$@ $(pkg_name)$(PKG_OUT_FLAGS)

   pkg_$(platform): $(ROOT)/$(PKGDIR)/$(pkg_name)$(PKG_OUT_FLAGS)$(PKG_OUT_EXT)
endef

# ====== Utilities ======

$(LOOKUPS): $(LOOKUPS_C_FILES) | $(BINDIR)/$(STAMPFILE)
	$(HOST_CC) $(CFLAGS_LOOKUPS) -o $@ $^

$(HEADPACK): $(HEADPACK_C_FILES) | $(BINDIR)/$(STAMPFILE)
	echo "Building $(HEADPACK)..."
	$(HOST_CC) $(CFLAGS_HEADPACK) -o $@ $^

$(MAP2ASN): $(MAP2ASN_C_FILES) | $(BINDIR)/$(STAMPFILE)
	$(HOST_CC) $(CFLAGS_MAP2ASN) -o $@ $^

$(COMPILE): $(COMPILE_C_FILES) | $(BINDIR)/$(STAMPFILE)
	$(HOST_CC) $(CFLAGS_COMPILE) -o $@ $^

