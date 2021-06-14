
CC := wcc
LD := wcl
CFLAGS := -0
LDFLAGS :=

DSEKAI_C_FILES := main.c graphics.c
DSEKAI_O_FILES := $(DSEKAI_C_FILES:%.c=%.o)

all: dsekai

dsekai: $(DSEKAI_O_FILES)
	$(LD) $(LDFLAGS) -fe=$@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(<:%.o=%)

