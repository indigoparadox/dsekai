
DSEKAI_C_FILES := main.c tilemap.c graphics.c mobile.c item.c window.c

DSEKAI_C_FILES_LINUX := input/sdli.c graphics/sdlg.c
DSEKAI_C_FILES_DOS := input/dosi.c graphics/dosg.c

BINDIR := ../bin

BIN_LINUX := $(BINDIR)/dsekai
BIN_DOS := $(BINDIR)/dsekai.exe

MD := mkdir -p

$(BIN_LINUX): CFLAGS := -DUSE_SDL -DSCREEN_SCALE=3 -DIGNORE_DIRTY $(shell pkg-config sdl2 --cflags)
$(BIN_LINUX): LDFLAGS := $(shell pkg-config sdl2 --libs)

$(BIN_DOS): CC := wcc
$(BIN_DOS): LD := wcl
$(BIN_DOS): CFLAGS := -0 -DUSE_DOS -mm -DSCALE_2X -DUSE_LOOKUPS
$(BIN_DOS): LDFLAGS := $(CFLAGS)

OBJDIR_LINUX := ../obj/linux/
OBJDIR_DOS := ../obj/dos/

DSEKAI_O_FILES_LINUX := $(addprefix $(OBJDIR_LINUX),$(subst .c,.o,$(DSEKAI_C_FILES))) $(addprefix $(OBJDIR_LINUX),$(subst .c,.o,$(DSEKAI_C_FILES_LINUX)))
DSEKAI_O_FILES_DOS := $(addprefix $(OBJDIR_DOS),$(subst .c,.o,$(DSEKAI_C_FILES))) $(addprefix $(OBJDIR_DOS),$(subst .c,.o,$(DSEKAI_C_FILES_DOS)))

.PHONY: clean

all: $(BIN_DOS) $(BIN_LINUX) ../bin/lookup

$(BIN_DOS): $(DSEKAI_O_FILES_DOS)
	$(MD) $(BINDIR)
	$(LD) $(LDFLAGS) -fe=$@ $^

$(BIN_LINUX): $(DSEKAI_O_FILES_LINUX)
	$(MD) $(BINDIR)
	$(CC) -o $@ $^ $(LDFLAGS)

$(BINDIR)/lookup: lookup.c
	gcc -o $@ $^

../obj/dos/%.o: %.c
	$(MD) $(OBJDIR)$(dir $@)
	$(CC) $(CFLAGS) -fo=$(OBJDIR)$@ $(<:%.c=%)

../obj/linux/%.o: %.c
	$(MD) $(OBJDIR)$(dir $@)
	$(CC) $(CFLAGS) -c -o $(OBJDIR)$@ $(<:%.o=%)

clean:
	rm -rf ../obj ../bin ../lookup *.err

