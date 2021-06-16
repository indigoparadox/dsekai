
CC := wcc
LD := wcl
#CFLAGS := -0 -mh -DUSE_LOOKUPS
#LDFLAGS := -mh
CFLAGS := -0 -DUSE_DOS -mm -DSCALE_2X -DUSE_LOOKUPS
LDFLAGS := $(CFLAGS)

DSEKAI_C_FILES := main.c graphics/dosg.c tilemap.c input/dosi.c graphics.c mobile.c item.c
DSEKAI_O_FILES := $(patsubst .c,.o,$(DSEKAI_C_FILES))

.PHONY: clean

all: dsekai

dsekai: $(DSEKAI_O_FILES)
	$(LD) $(LDFLAGS) -fe=$@ $^

lookup: lookup.c
	gcc -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(<:%.o=%)

clean:
	rm *.o *.exe lookup *.err
