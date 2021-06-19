
DSEKAI_C_FILES := tilemap.c graphics.c mobile.c item.c window.c topdown.c

DSEKAI_C_FILES_LINUX := input/sdli.c graphics/sdlg.c main.c
DSEKAI_C_FILES_DOS := input/dosi.c graphics/dosg.c main.c
DSEKAI_C_FILES_PALM := input/palmi.c graphics/palmg.c mainpalm.c

DSEKAI_ASSET_HEADERS := data/sprites.h

DSEKAI_ASSET_HEADERS_PALM := $(subst .h,_v.h,$(DSEKAI_ASSET_HEADERS))

BINDIR := ../bin

BIN_LINUX := $(BINDIR)/dsekai
BIN_DOS := $(BINDIR)/dsekai.exe
BIN_PALM := $(BINDIR)/dsekai.prc

MD := mkdir -p

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
$(BIN_PALM): CFLAGS := -O0 -DUSE_PALM -DSCREEN_W=160 -DSCREEN_H=160 $(INCLUDES)
$(BIN_PALM): ICONTEXT := "dsekai"
$(BIN_PALM): APPID := DSEK

OBJDIR_LINUX := ../obj/linux/
OBJDIR_DOS := ../obj/dos/
OBJDIR_PALM := ../obj/palm/

DSEKAI_O_FILES_LINUX := $(addprefix $(OBJDIR_LINUX),$(subst .c,.o,$(DSEKAI_C_FILES))) $(addprefix $(OBJDIR_LINUX),$(subst .c,.o,$(DSEKAI_C_FILES_LINUX)))
DSEKAI_O_FILES_DOS := $(addprefix $(OBJDIR_DOS),$(subst .c,.o,$(DSEKAI_C_FILES))) $(addprefix $(OBJDIR_DOS),$(subst .c,.o,$(DSEKAI_C_FILES_DOS)))
DSEKAI_O_FILES_PALM := $(addprefix $(OBJDIR_PALM),$(subst .c,.o,$(DSEKAI_C_FILES))) $(addprefix $(OBJDIR_PALM),$(subst .c,.o,$(DSEKAI_C_FILES_PALM)))

.PHONY: clean

all: $(BIN_DOS) $(BIN_LINUX) ../bin/lookup

$(BIN_DOS): $(DSEKAI_O_FILES_DOS)
	$(MD) $(BINDIR)
	$(LD) $(LDFLAGS) -fe=$@ $^

$(BIN_LINUX): $(DSEKAI_O_FILES_LINUX)
	$(MD) $(BINDIR)
	$(CC) -o $@ $^ $(LDFLAGS)

$(BIN_PALM): $(OBJDIR_PALM)grc.stamp $(OBJDIR_PALM)bin.stamp
	$(MD) $(BINDIR)
	$(BUILDPRC) $(BIN_PALM) $(ICONTEXT) $(APPID) $(OBJDIR_PALM)*.grc $(OBJDIR_PALM)*.bin $(LINKFILES) 

$(OBJDIR_PALM)grc.stamp: $(OBJDIR_PALM)dsekai
	cd $(OBJDIR_PALM) && $(OBJRES) dsekai
	touch $@

$(OBJDIR_PALM)mainpalm.o: $(DSEKAI_ASSET_HEADERS_PALM)

$(OBJDIR_PALM)dsekai: $(DSEKAI_O_FILES_PALM)
	$(CC) $(CFLAGS) $^ -o $@
	
$(OBJDIR_PALM)bin.stamp: palm.rcp
	$(PILRC) $^ $(OBJDIR_PALM)
	touch $@
	
data/sprites_v.h: data/sprites.h
	python ../scripts/cga2bmp.py -if $^ -of $@ -s 16 16

$(BINDIR)/lookup: lookup.c
	gcc -o $@ $^

../obj/dos/%.o: %.c
	$(MD) $(dir $@)
	$(CC) $(CFLAGS) -fo=$@ $(<:%.c=%)

../obj/linux/%.o: %.c
	$(MD) $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $(<:%.o=%)

../obj/palm/%.o: %.c
	$(MD) $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $(<:%.o=%)

clean:
	rm -rf data/*_v.h ../obj ../bin ../lookup *.err

