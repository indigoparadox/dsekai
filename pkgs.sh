#!/bin/bash

build_sdl() {
   make -f Makefile.sdl DTHRESHOLD=$DEBUG_THRESHOLD BUILD=$BUILD pkg_sdl || exit
   make -f Makefile.sdl DTHRESHOLD=$DEBUG_THRESHOLD DEPTH=VGA RESOURCE=FILE FMT_ASN=TRUE BUILD=$BUILD pkg_sdl || exit
   make -f Makefile.sdl DTHRESHOLD=$DEBUG_THRESHOLD DEPTH=VGA RESOURCE=FILE FMT_JSON=TRUE BUILD=$BUILD pkg_sdl || exit
}

build_dos() {
   make -f Makefile.dos DTHRESHOLD=$DEBUG_THRESHOLD RESOURCE=FILE FMT_ASN=TRUE BUILD=$BUILD ARCFMT=ZIP pkg_dos || exit
}

build_win() {
   make -f Makefile.win16 DTHRESHOLD=$DEBUG_THRESHOLD DEPTH=VGA RESOURCE=FILE FMT_ASN=TRUE BUILD=$BUILD ARCFMT=ZIP pkg_win16 || exit
   make -f Makefile.win32 DTHRESHOLD=$DEBUG_THRESHOLD DEPTH=VGA RESOURCE=FILE FMT_JSON=TRUE BUILD=$BUILD ARCFMT=ZIP pkg_win32 || exit
}

if [ "$1" = "run" ]; then
   PLAT_SPEC=$2
   BUILD=DEBUG
   DEBUG_THRESHOLD=1
   
   if [ "$PLAT_SPEC" = "sdl" ]; then
      if [ ! -d pkgbuild/dsekai-sdl-*-vga-*-asn ]; then
         build_sdl
      fi
      cd pkgbuild/dsekai-sdl-*-vga-*-asn && ./dsekai
   elif [ "$PLAT_SPEC" = "dos" ]; then
      if [ ! -d pkgbuild/dsekai-dos-*-cga-*-asn ]; then
         build_dos
      fi
      # This is complicated by FreeDOS for /D not finding wildcards...
      # Requires a symlink "dsekai" to this directory in dosemu root.
      if [ ! -L "dsdos" ]; then
         ln -s pkgbuild/dsekai-dos-*-cga-*-asn/ dsdos
      fi
      echo "cd \\dsekai\\dsdos" > dsdos/dsdos.bat
      echo "dsekai.exe" >> dsdos/dsdos.bat
      dosemu -2 "dsekai\\dsdos\\dsdos.bat"
   elif [ "$PLAT_SPEC" = "win32" ]; then
      if [ ! -d pkgbuild/dsekai-win32-*-vga-*-json ]; then
         build_win
      fi
      cd pkgbuild/dsekai-win32-*-vga-*-json && wine ./dsekai32.exe
   elif [ "$PLAT_SPEC" = "win16" ]; then
      if [ ! -d pkgbuild/dsekai-win16-*-vga-*-asn ]; then
         build_win
      fi
      cd pkgbuild/dsekai-win16-*-vga-*-asn && wine ./dsekai16.exe
   fi
   
   exit
elif [ "$1" = "RELEASE" ]; then
   BUILD=RELEASE
   PLAT_SPEC=$2
else
   BUILD=DEBUG
   PLAT_SPEC=$1
   DEBUG_THRESHOLD=1
fi

if [ "$PLAT_SPEC" = "sdl" ] || [ -z "$PLAT_SPEC" ]; then
   build_sdl
fi

if [ "$PLAT_SPEC" = "palm" ] || [ -z "$PLAT_SPEC" ]; then
   make -f Makefile.palm DTHRESHOLD=$DEBUG_THRESHOLD BUILD=$BUILD DEPTH=MONO pkg_palm || exit
fi

if [ "$PLAT_SPEC" = "xlib" ] || [ -z "$PLAT_SPEC" ]; then
   make -f Makefile.xlib DTHRESHOLD=$DEBUG_THRESHOLD RESOURCE=FILE FMT_JSON=TRUE BUILD=$BUILD pkg_xlib || exit
fi

if [ "$PLAT_SPEC" = "dos" ] || [ -z "$PLAT_SPEC" ]; then
   build_dos
fi

if [ "$PLAT_SPEC" = "win" ] || [ -z "$PLAT_SPEC" ]; then
   build_win
fi

if [ "$PLAT_SPEC" = "wasm" ] || [ -z "$PLAT_SPEC" ]; then
   make -f Makefile.wasm DTHRESHOLD=$DEBUG_THRESHOLD DEPTH=VGA BUILD=$BUILD ARCFMT=ZIP pkg_wasm || exit
   make -f Makefile.wasm DTHRESHOLD=$DEBUG_THRESHOLD BUILD=$BUILD ARCFMT=ZIP pkg_wasm || exit
fi

if [ "$PLAT_SPEC" = "mac" ] || [ -z "$PLAT_SPEC" ]; then
   make -f Makefile.mac6 DTHRESHOLD=$DEBUG_THRESHOLD DEPTH=MONO FMT_ASN=TRUE RESOURCE=FILE bin-file-mono-asn/dsekai.APPL || exit
fi

echo "All packages built OK!"

