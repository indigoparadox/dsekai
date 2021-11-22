#!/bin/bash
BUILD=DEBUG

if [ "$1" = "sdl" ] || [ -z "$1" ]; then
   make -f Makefile.sdl BUILD=$BUILD pkg_sdl || exit
   make -f Makefile.sdl DEPTH=VGA RESOURCE=FILE FMT_ASN=TRUE BUILD=$BUILD pkg_sdl || exit
   make -f Makefile.sdl DEPTH=VGA RESOURCE=FILE FMT_JSON=TRUE BUILD=$BUILD pkg_sdl || exit
fi

if [ "$1" = "palm" ] || [ -z "$1" ]; then
   make -f Makefile.palm BUILD=$BUILD DEPTH=MONO pkg_palm || exit
fi

if [ "$1" = "xlib" ] || [ -z "$1" ]; then
   make -f Makefile.xlib RESOURCE=FILE FMT_JSON=TRUE BUILD=$BUILD pkg_xlib || exit
fi

if [ "$1" = "dos" ] || [ -z "$1" ]; then
   make -f Makefile.dos RESOURCE=FILE FMT_ASN=TRUE BUILD=$BUILD ARCFMT=ZIP pkg_dos || exit
fi

if [ "$1" = "win" ] || [ -z "$1" ]; then
   make -f Makefile.win16 DEPTH=VGA RESOURCE=FILE FMT_ASN=TRUE BUILD=$BUILD ARCFMT=ZIP pkg_win16 || exit
   make -f Makefile.win32 DEPTH=VGA RESOURCE=FILE FMT_JSON=TRUE BUILD=$BUILD ARCFMT=ZIP pkg_win32 || exit
fi

if [ "$1" = "mac" ] || [ -z "$1" ]; then
   make -f Makefile.mac6 DEPTH=MONO FMT_ASN=TRUE RESOURCE=FILE bin-file-mono/dsekai.APPL || exit
fi

