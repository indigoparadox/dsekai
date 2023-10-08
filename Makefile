
# vim: ft=make noexpandtab

VDP=1
#WING=1
C_FILES := src/crop.c src/engines.c src/irc.c src/iso.c src/item.c src/main.c src/menu.c src/mobile.c src/pathfind.c src/scparse.c src/script.c src/strpool.c src/tilemap.c src/title.c src/tmasn.c src/tmjson.c src/topdown.c

include maug/Makefile.inc

# Target-specific options.
.PHONY: clean

all: dsekai.sdl dsekai.ale dsekaid.exe dsekaint.exe dsekai.html dsekaiw3.exe

# Nintendo DS

#$(eval $(call TGTNDSLIBN,dsekai,dsekai.bmp))

# Unix Allegro

$(eval $(call TGTUNIXALE,dsekai))

# Unix SDL

#$(eval $(call TGTSDLICO,dsekai))

$(eval $(call TGTUNIXSDL,dsekai))

# WASM

$(eval $(call TGTWASMSDL,dsekai))

# DOS

$(eval $(call TGTDOSALE,dsekai))

$(eval $(call TGTDOSCGA,dsekai))

# Win386

$(eval $(call TGTWINICO,dsekai))

$(eval $(call TGTWIN386,dsekai))

$(eval $(call TGTWIN16,dsekai))

# WinNT

$(eval $(call TGTWINNT,dsekai))

$(eval $(call TGTWINSDL,dsekai))

# OS/2

$(eval $(call TGTOS2SDL,dsekai))

# Mac

$(eval $(call TGTMAC6GCC,dsekai))

# Clean

clean:
	rm -rf $(CLEAN_TARGETS)

