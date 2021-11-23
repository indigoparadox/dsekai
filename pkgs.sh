#!/bin/bash
BUILD=DEBUG
DEBUG_THRESHOLD=1

if [ "$1" = "sdl" ] || [ -z "$1" ]; then
   make -f Makefile.sdl DTHRESHOLD=$DEBUG_THRESHOLD BUILD=$BUILD pkg_sdl || exit
   make -f Makefile.sdl DTHRESHOLD=$DEBUG_THRESHOLD DEPTH=VGA RESOURCE=FILE FMT_ASN=TRUE BUILD=$BUILD pkg_sdl || exit
   make -f Makefile.sdl DTHRESHOLD=$DEBUG_THRESHOLD DEPTH=VGA RESOURCE=FILE FMT_JSON=TRUE BUILD=$BUILD pkg_sdl || exit
fi

if [ "$1" = "palm" ] || [ -z "$1" ]; then
   make -f Makefile.palm DTHRESHOLD=$DEBUG_THRESHOLD BUILD=$BUILD DEPTH=MONO pkg_palm || exit
fi

if [ "$1" = "xlib" ] || [ -z "$1" ]; then
   make -f Makefile.xlib DTHRESHOLD=$DEBUG_THRESHOLD RESOURCE=FILE FMT_JSON=TRUE BUILD=$BUILD pkg_xlib || exit
fi

if [ "$1" = "dos" ] || [ -z "$1" ]; then
   make -f Makefile.dos DTHRESHOLD=$DEBUG_THRESHOLD RESOURCE=FILE FMT_ASN=TRUE BUILD=$BUILD ARCFMT=ZIP pkg_dos || exit
fi

if [ "$1" = "win" ] || [ -z "$1" ]; then
   make -f Makefile.win16 DTHRESHOLD=$DEBUG_THRESHOLD DEPTH=VGA RESOURCE=FILE FMT_ASN=TRUE BUILD=$BUILD ARCFMT=ZIP pkg_win16 || exit
   make -f Makefile.win32 DTHRESHOLD=$DEBUG_THRESHOLD DEPTH=VGA RESOURCE=FILE FMT_JSON=TRUE BUILD=$BUILD ARCFMT=ZIP pkg_win32 || exit
fi

if [ "$1" = "mac" ] || [ -z "$1" ]; then
   make -f Makefile.mac6 DTHRESHOLD=$DEBUG_THRESHOLD DEPTH=MONO FMT_ASN=TRUE RESOURCE=FILE bin-file-mono/dsekai.APPL || exit
fi

