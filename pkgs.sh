#!/bin/bash
BUILD=DEBUG
make -f Makefile.sdl RESOURCE=FILE FMT_JSON=TRUE BUILD=$BUILD pkg_sdl && \
make -f Makefile.sdl DEPTH=VGA RESOURCE=FILE FMT_JSON=TRUE BUILD=$BUILD pkg_sdl && \
make -f Makefile.palm BUILD=$BUILD DEPTH=MONO pkg_palm && \
make -f Makefile.xlib RESOURCE=FILE FMT_JSON=TRUE BUILD=$BUILD pkg_xlib && \
make -f Makefile.dos RESOURCE=FILE FMT_ASN=TRUE BUILD=$BUILD ARCFMT=ZIP pkg_dos && \
make -f Makefile.win16 DEPTH=VGA RESOURCE=FILE FMT_ASN=TRUE BUILD=$BUILD ARCFMT=ZIP pkg_win16 && \
make -f Makefile.win32 DEPTH=VGA RESOURCE=FILE FMT_JSON=TRUE BUILD=$BUILD ARCFMT=ZIP pkg_win32

