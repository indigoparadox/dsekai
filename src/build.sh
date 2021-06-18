#!/bin/sh

if [ "$1" = "linux" ]; then
   gcc -O0 -o isekai \
      -DUSE_SDL -DSCREEN_SCALE=3 -DIGNORE_DIRTY \
      main.c tilemap.c graphics/sdlg.c input/sdli.c graphics.c mobile.c item.c window.c \
      `pkg-config sdl2 --cflags --libs` && \
      ./isekai
elif [ "$1" = "dos" ]; then
   rm */*.o *.exe *.o
   INCLUDE=/opt/watcom/h/ WATCOM=/opt/watcom make && \
   dosemu -2 ./dsekai.exe
fi

