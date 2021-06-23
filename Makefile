
DSEKAI_C_FILES := \
   src/tilemap.c \
   src/graphics.c \
   src/mobile.c \
   src/item.c \
   src/window.c \
   src/topdown.c \
   src/psprintf.c

DSEKAI_C_FILES_LINUX := src/input/sdli.c src/graphics/sdlg.c src/main.c
DSEKAI_C_FILES_DOS := src/input/dosi.c src/graphics/dosg.c src/main.c
DSEKAI_C_FILES_PALM := src/input/palmi.c src/graphics/palmg.c src/mainpalm.c
DSEKAI_C_FILES_WIN16 := src/input/win16i.c src/graphics/win16g.c src/mainw16.c
DSEKAI_C_FILES_CHECK := \
   check/check.c \
   check/check_mobile.c \
   check/check_item.c \
   check/check_tilemap.c \
   check/check_window.c \
   check/check_psprintf.c \
   check/check_graphics.c \
   check/check_engines.c

DSEKAI_ASSET_HEADERS := src/data/sprites.h src/data/tilebmps.h
DSEKAI_ASSET_DIMENSION := 16 16

RESEXT_H: src/resext.h
TOPDOWN_O: src/topdown.o

ASSETDIR_PALM := gen/palm
ASSETDIR_WIN16 := gen/win16

BINDIR := bin

BIN_LINUX := $(BINDIR)/dsekai
BIN_DOS := $(BINDIR)/dsekai.exe
BIN_PALM := $(BINDIR)/dsekai.prc
BIN_WIN16 := $(BINDIR)/dsekai16.exe

BIN_CHECK_LINUX := $(BINDIR)/check

MD := mkdir -p
PYTHON := python
CGA2BMP := scripts/cga2bmp.py

$(BIN_LINUX): CFLAGS := -DUSE_SDL -DSCREEN_SCALE=3 $(shell pkg-config sdl2 --cflags) -g -DSCREEN_W=160 -DSCREEN_H=160 -Wall -Wno-missing-braces -Wno-char-subscripts -std=c89
$(BIN_LINUX): LDFLAGS := $(shell pkg-config sdl2 --libs) -g

$(BIN_DOS): CC := wcc
$(BIN_DOS): LD := wcl
$(BIN_DOS): CFLAGS := -0 -DUSE_DOS -mm -DSCALE_2X -DUSE_LOOKUPS
$(BIN_DOS): LDFLAGS := $(CFLAGS)

$(BIN_PALM): CC := m68k-palmos-gcc
$(BIN_PALM): PILRC := pilrc
$(BIN_PALM): TXT2BITM := txt2bitm
$(BIN_PALM): OBJRES := m68k-palmos-obj-res
$(BIN_PALM): BUILDPRC := build-prc
$(BIN_PALM): INCLUDES := -I /opt/palmdev/sdk-3.5/include -I /opt/palmdev/sdk-3.5/include/Core/UI/ -I /opt/palmdev/sdk-3.5/include/Core/System/ -I /opt/palmdev/sdk-3.5/include/Core/Hardware/ -I /opt/palmdev/sdk-3.5/include/Core/International/
$(BIN_PALM): CFLAGS := -O0 -DUSE_PALM -DSCREEN_W=160 -DSCREEN_H=160 $(INCLUDES) -DHIDE_WELCOME_DIALOG -DNO_PALM_DEBUG_LINE -g
$(BIN_PALM): LDFLAGS = -g
$(BIN_PALM): ICONTEXT := "dsekai"
$(BIN_PALM): APPID := DSEK
$(BIN_PALM): PALMS_RCP := src/palms.rcp

$(BIN_WIN16): CC := wcc
$(BIN_WIN16): LD := wcl
$(BIN_WIN16): RC := wrc
$(BIN_WIN16): CFLAGS := -bt=windows -i=$(INCLUDE)/win -DUSE_WIN16 -DSCALE_2X -DUSE_LOOKUPS
$(BIN_WIN16): LDFLAGS := -l=windows

$(BIN_CHECK_LINUX): CFLAGS := -DUSE_NULL -DSCREEN_SCALE=3 $(shell pkg-config check --cflags) -g -DSCREEN_W=160 -DSCREEN_H=160 -Wall -Wno-missing-braces -Wno-char-subscripts -std=c89
$(BIN_CHECK_LINUX): LDFLAGS := $(shell pkg-config check --libs) -g

OBJDIR_LINUX := obj/linux/
OBJDIR_DOS := obj/dos/
OBJDIR_PALM := obj/palm/
OBJDIR_WIN16 := obj/win16/
OBJDIR_CHECK_LINUX := obj/check_linux/

DSEKAI_O_FILES_LINUX := $(addprefix $(OBJDIR_LINUX),$(subst .c,.o,$(DSEKAI_C_FILES))) $(addprefix $(OBJDIR_LINUX),$(subst .c,.o,$(DSEKAI_C_FILES_LINUX)))
DSEKAI_O_FILES_DOS := $(addprefix $(OBJDIR_DOS),$(subst .c,.o,$(DSEKAI_C_FILES))) $(addprefix $(OBJDIR_DOS),$(subst .c,.o,$(DSEKAI_C_FILES_DOS)))
DSEKAI_O_FILES_PALM := $(addprefix $(OBJDIR_PALM),$(subst .c,.o,$(DSEKAI_C_FILES))) $(addprefix $(OBJDIR_PALM),$(subst .c,.o,$(DSEKAI_C_FILES_PALM)))
DSEKAI_O_FILES_WIN16 := $(addprefix $(OBJDIR_WIN16),$(subst .c,.o,$(DSEKAI_C_FILES))) $(addprefix $(OBJDIR_WIN16),$(subst .c,.o,$(DSEKAI_C_FILES_WIN16)))
DSEKAI_O_FILES_CHECK_LINUX := $(addprefix $(OBJDIR_CHECK_LINUX),$(subst .c,.o,$(DSEKAI_C_FILES))) $(addprefix $(OBJDIR_CHECK_LINUX),$(subst .c,.o,$(DSEKAI_C_FILES_CHECK)))

.PHONY: clean

all: $(BIN_DOS) $(BIN_LINUX) bin/lookup

bin/drcpack: src/drcpack.c src/data/drc.c src/data/dio.c
	$(MD) $(BINDIR)
	$(CC) -g -o $@ $^ $(LDFLAGS)

bin/convert: src/convert.c src/data/bmp.c src/data/drc.c src/data/cga.c src/data/dio.c
	$(MD) $(BINDIR)
	$(CC) -g -o $@ $^ $(LDFLAGS)

bin/editor: src/editor.c src/data/bmp.c src/data/pak.c
	$(MD) $(BINDIR)
	$(CC) -o $@ $^ $(LDFLAGS)

$(BIN_LINUX): $(DSEKAI_O_FILES_LINUX)
	$(MD) $(BINDIR)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJDIR_LINUX)$(TOPDOWN_O): $(RESEXT_H)

$(RESEXT_H): $(DSEKAI_ASSET_HEADERS)
	python $(CGA2BMP) -if $^ \
      -s $(DSEKAI_ASSET_DIMENSION) -ei l -bi 2 -c none \
      -re $@

$(BIN_DOS): $(DSEKAI_O_FILES_DOS)
	$(MD) $(BINDIR)
	$(LD) $(LDFLAGS) -fe=$@ $^

$(OBJDIR_DOS)$(TOPDOWN_O): $(RESEXT_H)

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

$(OBJDIR_CHECK_LINUX)$(TOPDOWN_O): $(RESEXT_H)

$(BIN_CHECK_LINUX): $(DSEKAI_O_FILES_CHECK_LINUX)
	$(MD) $(BINDIR)
	$(CC) -o $@ $^ $(LDFLAGS)

$(BINDIR)/lookup: lookup.c
	gcc -o $@ $^

$(OBJDIR_DOS)%.o: %.c
	$(MD) $(dir $@)
	$(CC) $(CFLAGS) -fo=$@ $(<:%.c=%)

$(OBJDIR_LINUX)%.o: %.c
	$(MD) $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $(<:%.o=%)

$(OBJDIR_PALM)%.o: %.c
	$(MD) $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $(<:%.o=%)

$(OBJDIR_WIN16)%.o: %.c
	$(MD) $(dir $@)
	$(CC) $(CFLAGS) -fo=$@ $(<:%.c=%)

$(OBJDIR_CHECK_LINUX)%.o: %.c
	$(MD) $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $(<:%.o=%)

clean:
	rm -rf $(RESEXT_H) data obj bin *.err

