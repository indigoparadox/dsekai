
DSEKAI_C_FILES := \
   src/tilemap.c \
   src/graphics.c \
   src/mobile.c \
   src/item.c \
   src/window.c \
   src/topdown.c \
   src/data/dio.c \
   src/data/drc.c

DSEKAI_C_FILES_SDL := src/input/sdli.c src/graphics/sdlg.c src/main.c
DSEKAI_C_FILES_DOS := src/input/dosi.c src/graphics/dosg.c src/main.c
DSEKAI_C_FILES_PALM := src/input/palmi.c src/graphics/palmg.c src/mainpalm.c
DSEKAI_C_FILES_WIN16 := src/input/win16i.c src/graphics/win16g.c src/mainw16.c
DSEKAI_C_FILES_CHECK := \
   check/check.c \
   check/check_mobile.c \
   check/check_item.c \
   check/check_tilemap.c \
   check/check_window.c \
   check/check_graphics.c \
   check/check_engines.c

CONVERT_C_FILES := \
   src/convert.c \
   src/data/bmp.c \
   src/data/drc.c \
   src/data/cga.c \
   src/data/dio.c

DSEKAI_ASSET_HEADERS := src/data/sprites.h src/data/tilebmps.h
DSEKAI_ASSET_DIMENSION := 16 16

TOPDOWN_O: src/topdown.o

ASSETDIR := assets

ASSETDIR_PALM := gen/palm
ASSETDIR_WIN16 := gen/win16

OBJDIR_SDL := obj/sdl/
OBJDIR_DOS := obj/dos/
OBJDIR_PALM := obj/palm/
OBJDIR_WIN16 := obj/win16/
OBJDIR_CHECK_NULL := obj/check_null/

GENDIR_SDL := gen/sdl/
GENDIR_DOS := gen/dos/

BINDIR := bin

BIN_SDL := $(BINDIR)/dsekai
BIN_DOS := $(BINDIR)/dsekai.exe
BIN_PALM := $(BINDIR)/dsekai.prc
BIN_WIN16 := $(BINDIR)/dsekai16.exe

BIN_CHECK_NULL := $(BINDIR)/check

DSEKAI_ASSETS_BMP := $(wildcard $(ASSETDIR)/*.bmp)
DSEKAI_ASSETS_DOS_CGA := \
   $(subst .bmp,.cga,$(subst $(ASSETDIR)/,$(GENDIR_DOS),$(DSEKAI_ASSETS_BMP)))

MD := mkdir -p
PYTHON := python
CGA2BMP := scripts/cga2bmp.py
DRCPACK := bin/drcpack
CONVERT := bin/convert

$(BIN_SDL): CFLAGS := -DSCREEN_SCALE=3 $(shell pkg-config sdl2 --cflags) -g -DSCREEN_W=160 -DSCREEN_H=160 -Wall -Wno-missing-braces -Wno-char-subscripts -std=c89 -DPLATFORM_SDL -fsanitize=address -fsanitize=leak -DDIO_SILENT
$(BIN_SDL): LDFLAGS := $(shell pkg-config sdl2 --libs) -g -fsanitize=address -fsanitize=leak

$(BIN_DOS): CC := wcc
$(BIN_DOS): LD := wcl
$(BIN_DOS): CFLAGS := -hw -d3 -0 -mm -DSCALE_2X -DUSE_LOOKUPS -DPLATFORM_DOS -DDIO_SILENT
$(BIN_DOS): LDFLAGS := $(CFLAGS)

$(BIN_PALM): CC := m68k-palmos-gcc
$(BIN_PALM): PILRC := pilrc
$(BIN_PALM): TXT2BITM := txt2bitm
$(BIN_PALM): OBJRES := m68k-palmos-obj-res
$(BIN_PALM): BUILDPRC := build-prc
$(BIN_PALM): INCLUDES := -I /opt/palmdev/sdk-3.5/include -I /opt/palmdev/sdk-3.5/include/Core/UI/ -I /opt/palmdev/sdk-3.5/include/Core/System/ -I /opt/palmdev/sdk-3.5/include/Core/Hardware/ -I /opt/palmdev/sdk-3.5/include/Core/International/
$(BIN_PALM): CFLAGS := -O0 -DSCREEN_W=160 -DSCREEN_H=160 $(INCLUDES) -DHIDE_WELCOME_DIALOG -DNO_PALM_DEBUG_LINE -DPLATFORM_PALM -g
$(BIN_PALM): LDFLAGS = -g
$(BIN_PALM): ICONTEXT := "dsekai"
$(BIN_PALM): APPID := DSEK
$(BIN_PALM): PALMS_RCP := src/palms.rcp

$(BIN_WIN16): CC := wcc
$(BIN_WIN16): LD := wcl
$(BIN_WIN16): RC := wrc
$(BIN_WIN16): CFLAGS := -bt=windows -i=$(INCLUDE)/win -DSCALE_2X -DUSE_LOOKUPS -DPLATFORM_WIN16
$(BIN_WIN16): LDFLAGS := -l=windows

$(BIN_CHECK_NULL): CFLAGS := -DSCREEN_SCALE=3 $(shell pkg-config check --cflags) -g -DSCREEN_W=160 -DSCREEN_H=160 -Wall -Wno-missing-braces -Wno-char-subscripts -std=c89 -DPLATFORM=null
$(BIN_CHECK_NULL): LDFLAGS := $(shell pkg-config check --libs) -g

DSEKAI_O_FILES_SDL := $(addprefix $(OBJDIR_SDL),$(subst .c,.o,$(DSEKAI_C_FILES))) $(addprefix $(OBJDIR_SDL),$(subst .c,.o,$(DSEKAI_C_FILES_SDL)))
DSEKAI_O_FILES_DOS := $(addprefix $(OBJDIR_DOS),$(subst .c,.o,$(DSEKAI_C_FILES))) $(addprefix $(OBJDIR_DOS),$(subst .c,.o,$(DSEKAI_C_FILES_DOS)))
DSEKAI_O_FILES_PALM := $(addprefix $(OBJDIR_PALM),$(subst .c,.o,$(DSEKAI_C_FILES))) $(addprefix $(OBJDIR_PALM),$(subst .c,.o,$(DSEKAI_C_FILES_PALM)))
DSEKAI_O_FILES_WIN16 := $(addprefix $(OBJDIR_WIN16),$(subst .c,.o,$(DSEKAI_C_FILES))) $(addprefix $(OBJDIR_WIN16),$(subst .c,.o,$(DSEKAI_C_FILES_WIN16)))
DSEKAI_O_FILES_CHECK_NULL := $(addprefix $(OBJDIR_CHECK_NULL),$(subst .c,.o,$(DSEKAI_C_FILES))) $(addprefix $(OBJDIR_CHECK_NULL),$(subst .c,.o,$(DSEKAI_C_FILES_CHECK)))

.PHONY: clean res_sdl16_drc res_doscga_drc

all: $(BIN_DOS) $(BIN_SDL) bin/lookup

# ====== Utilities ======

$(DRCPACK): src/drcpack.c src/data/drc.c src/data/dio.c
	$(MD) $(BINDIR)
	gcc -g -o $@ $^

bin/convert: $(CONVERT_C_FILES)
	$(MD) $(BINDIR)
	gcc -g -o $@ $^

bin/editor: src/editor.c src/data/bmp.c src/data/pak.c
	$(MD) $(BINDIR)
	$(CC) -o $@ $^ $(LDFLAGS)

# ====== Main: Linux ======

$(BINDIR)/sdl16.drc: res_sdl16_drc

res_sdl16_drc: $(DRCPACK)
	$(MD) $(GENDIR_SDL)
	$(DRCPACK) -c -a -af $(BINDIR)/sdl16.drc -t BMP1 -i 5001 \
      -if $(ASSETDIR)/*.bmp -lh $(GENDIR_SDL)resext.h

$(BIN_SDL): $(DSEKAI_O_FILES_SDL)
	$(MD) $(BINDIR)
	$(CC) -o $@ $^ $(LDFLAGS)

# ====== Main: MS-DOS ======

$(BINDIR)/doscga.drc: res_doscga_drc

res_doscga_drc: $(DRCPACK) $(DSEKAI_ASSETS_DOS_CGA)
	$(DRCPACK) -c -a -af $(BINDIR)/doscga.drc -t BMP1 -i 5001 \
      -if $(GENDIR_DOS)*.cga -lh $(GENDIR_DOS)resext.h

$(GENDIR_DOS)%.cga: $(ASSETDIR)/%.bmp $(BINDIR)/convert
	$(MD) $(GENDIR_DOS)
	./bin/convert -ic bitmap -oc cga -ob 2 -if $< -of $@ -og

$(BIN_DOS): $(DSEKAI_O_FILES_DOS)
	$(MD) $(BINDIR)
	$(LD) $(LDFLAGS) -fe=$@ $^

$(OBJDIR_DOS)$(TOPDOWN_O): $(RESEXT_H)

# ====== Main: Palm ======

$(BIN_PALM): $(OBJDIR_PALM)grc.stamp $(OBJDIR_PALM)bin.stamp
	$(MD) $(BINDIR)
	$(BUILDPRC) $(BIN_PALM) $(ICONTEXT) $(APPID) $(OBJDIR_PALM)*.grc $(OBJDIR_PALM)*.bin $(LINKFILES) 

$(OBJDIR_PALM)$(TOPDOWN_O): $(RESEXT_H) $(ASSETDIR_PALM)/palm_rc.h

$(OBJDIR_PALM)grc.stamp: $(OBJDIR_PALM)dsekai
	cd $(OBJDIR_PALM) && $(OBJRES) dsekai
	touch $@

$(OBJDIR_PALM)mainpalm.o: $(ASSETDIR_PALM)/palm_ids.h $(ASSETDIR_PALM)/palm_rc.h $(ASSETDIR_PALM)/palm.rcp

$(OBJDIR_PALM)dsekai: $(DSEKAI_O_FILES_PALM)
	$(CC) $(CFLAGS) $^ -o $@
	
$(OBJDIR_PALM)bin.stamp: src/palms.rcp
	$(PILRC) $^ $(OBJDIR_PALM)
	touch $@

$(ASSETDIR_PALM)/palm_ids.h: $(ASSETDIR_PALM)/generate.stamp

$(ASSETDIR_PALM)/palm_rc.h: $(ASSETDIR_PALM)/generate.stamp
	
$(ASSETDIR_PALM)/gc_%.bmp: $(ASSETDIR_PALM)/generate.stamp

$(ASSETDIR_PALM)/generate.stamp: $(DSEKAI_ASSET_HEADERS)
	$(MD) $(ASSETDIR_PALM)
	$(PYTHON) $(CGA2BMP) -if $^ -of $(ASSETDIR_PALM) \
      -s $(DSEKAI_ASSET_DIMENSION) -ei l -bi 2 -c bitmap \
      -r $(ASSETDIR_PALM)/palm.rcp -rf palm \
      -ri $(ASSETDIR_PALM)/palm_ids.h \
      -rc $(ASSETDIR_PALM)/palm_rc.h
	touch $@

# ====== Main: Win16 ======

$(BIN_WIN16): $(DSEKAI_O_FILES_WIN16) $(OBJDIR_WIN16)win16.res
	$(MD) $(BINDIR)
	$(LD) $(LDFLAGS) -fe=$@ $^

$(OBJDIR_WIN16)$(TOPDOWN_O): $(RESEXT_H)

$(OBJDIR_WIN16)win16.res: $(ASSETDIR_WIN16)/win16.rc
	$(RC) -r -i=$(INCLUDE)/win src/win16s.rc -fo=$@

$(ASSETDIR_WIN16)/win16.rc: $(DSEKAI_ASSET_HEADERS)
	$(MD) $(ASSETDIR_WIN16)
	$(PYTHON) $(CGA2BMP) -if $^ -of $(ASSETDIR_WIN16) \
      -s $(DSEKAI_ASSET_DIMENSION) -ei l -bi 2 -c bitmap \
      -r $(ASSETDIR_WIN16)/win16.rc -rf win16 \
      -ri $(ASSETDIR_WIN16)/win16_ids.h \
      -rc $(ASSETDIR_WIN16)/win16_rc.h
	touch $@

$(OBJDIR_CHECK_NULL)$(TOPDOWN_O): $(RESEXT_H)

$(BIN_CHECK_NULL): $(DSEKAI_O_FILES_CHECK_NULL)
	$(MD) $(BINDIR)
	$(CC) -o $@ $^ $(LDFLAGS)

$(BINDIR)/lookup: lookup.c
	gcc -o $@ $^

$(OBJDIR_DOS)%.o: %.c res_doscga_drc
	$(MD) $(dir $@)
	$(CC) $(CFLAGS) -fo=$@ $(<:%.c=%)

$(OBJDIR_SDL)%.o: %.c res_sdl16_drc
	$(MD) $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $(<:%.o=%)

$(OBJDIR_PALM)%.o: %.c
	$(MD) $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $(<:%.o=%)

$(OBJDIR_WIN16)%.o: %.c
	$(MD) $(dir $@)
	$(CC) $(CFLAGS) -fo=$@ $(<:%.c=%)

$(OBJDIR_CHECK_NULL)%.o: %.c
	$(MD) $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $(<:%.o=%)

clean:
	rm -rf data obj bin gen *.err

