
CC := wcc
LD := wcl
#CFLAGS := -0 -mh -DUSE_LOOKUPS
#LDFLAGS := -mh
CFLAGS := -0
LDFLAGS :=

DSEKAI_C_FILES := main.c graphics.c tilemap.c
DSEKAI_O_FILES := $(DSEKAI_C_FILES:%.c=%.o)

.PHONY: clean

all: dsekai

dsekai: $(DSEKAI_O_FILES)
	$(LD) $(LDFLAGS) -fe=$@ $^

lookup: lookup.c
	gcc -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(<:%.o=%)

clean:
	rm *.o *.exe lookup
