#!/bin/bash

PLATFORMS="dos win386 win16 win32 sdl xlib curses wasm mac palm nds"
BUILD=DEBUG
DEBUG_THRESHOLD=1
ACTION=BUILD
PKG_ISO=0
GIT_HASH="`git rev-parse --short HEAD`"

build_sdl() {
   make -f Makefile.sdl SDL_VER=2 DTHRESHOLD=$DEBUG_THRESHOLD BUILD=$BUILD pkg_sdl || exit
   make -f Makefile.sdl SDL_VER=2 DTHRESHOLD=$DEBUG_THRESHOLD DEPTH=VGA RESOURCE=FILE FMT_ASN=TRUE BUILD=$BUILD pkg_sdl || exit
}

build_dos() {
   make -f Makefile.dos DTHRESHOLD=$DEBUG_THRESHOLD RESOURCE=FILE FMT_ASN=TRUE BUILD=$BUILD ARCFMT=ZIP pkg_dos || exit
   make -f Makefile.dos DTHRESHOLD=$DEBUG_THRESHOLD DEPTH=VGA RESOURCE=FILE FMT_ASN=TRUE BUILD=$BUILD ARCFMT=ZIP pkg_dos || exit
}

build_win386() {
   make -f Makefile.win386 DTHRESHOLD=$DEBUG_THRESHOLD DEPTH=VGA RESOURCE=FILE FMT_ASN=TRUE BUILD=$BUILD ARCFMT=ZIP pkg_win386 || exit
}

build_win16() {
   make -f Makefile.win16 DTHRESHOLD=$DEBUG_THRESHOLD DEPTH=VGA RESOURCE=FILE FMT_ASN=TRUE BUILD=$BUILD ARCFMT=ZIP pkg_win16 || exit
}

build_win32() {
   make -f Makefile.win32 DTHRESHOLD=$DEBUG_THRESHOLD DEPTH=VGA RESOURCE=FILE FMT_ASN=TRUE BUILD=$BUILD ARCFMT=ZIP pkg_win32 || exit
}

build_xlib() {
   make -f Makefile.xlib DTHRESHOLD=$DEBUG_THRESHOLD RESOURCE=FILE FMT_ASN=TRUE BUILD=$BUILD pkg_xlib || exit
}

build_curses() {
   make -f Makefile.curses DTHRESHOLD=$DEBUG_THRESHOLD RESOURCE=FILE FMT_ASN=TRUE BUILD=$BUILD pkg_curses || exit
}

build_mac6() {
   make -f Makefile.mac6 DTHRESHOLD=$DEBUG_THRESHOLD DEPTH=MONO FMT_ASN=TRUE RESOURCE=FILE BUILD=$BUILD pkg_mac6 || exit
}

build_nds() {
   make -f Makefile.nds DTHRESHOLD=$DEBUG_THRESHOLD DEPTH=VGA FMT_ASN=TRUE BUILD=$BUILD pkg_nds || exit
}

do_run() {
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
      if [ ! -d pkgbuild/dsekai-win32-*-vga-*-asn ]; then
         build_win32
      fi
      cd pkgbuild/dsekai-win32-*-vga-*-asn && wine ./dsekai32.exe
   elif [ "$PLAT_SPEC" = "win16" ]; then
      if [ ! -d pkgbuild/dsekai-win16-*-vga-*-asn ]; then
         build_win16
      fi
      cd pkgbuild/dsekai-win16-*-vga-*-asn && wine ./dsekai16.exe
   elif [ "$PLAT_SPEC" = "win386" ]; then
      if [ ! -d pkgbuild/dsekai-win386-*-vga-*-asn ]; then
         build_win386
      fi
      cd pkgbuild/dsekai-win386-*-vga-*-asn && wine ./dsekaile.exe
   elif [ "$PLAT_SPEC" = "xlib" ]; then
      if [ ! -d pkgbuild/dsekai-xlib-*-cga-*-asn ]; then
         build_xlib
      fi
      cd pkgbuild/dsekai-xlib-*-cga-*-asn && ./dsekaix
   elif [ "$PLAT_SPEC" = "curses" ]; then
      if [ ! -d pkgbuild/dsekai-curses-* ]; then
         build_curses
      fi
      cd pkgbuild/dsekai-curses-* && ./dsekait
   elif [ "$PLAT_SPEC" = "wasm" ]; then
      cd pkgbuild/dsekai-wasm-*-vga-* && python -m http.server
   fi
}

do_build() {
   if [ "$PLAT_SPEC" = "sdl" ] || [ -z "$PLAT_SPEC" ]; then
      build_sdl
   fi

   if [ "$PLAT_SPEC" = "palm" ] || [ -z "$PLAT_SPEC" ]; then
      make -f Makefile.palm DTHRESHOLD=$DEBUG_THRESHOLD BUILD=$BUILD DEPTH=MONO pkg_palm || exit
   fi

   if [ "$PLAT_SPEC" = "xlib" ] || [ -z "$PLAT_SPEC" ]; then
      build_xlib
   fi

   if [ "$PLAT_SPEC" = "curses" ] || [ -z "$PLAT_SPEC" ]; then
      build_curses
   fi

   if [ "$PLAT_SPEC" = "dos" ] || [ -z "$PLAT_SPEC" ]; then
      build_dos
   fi

   if [ "$PLAT_SPEC" = "win386" ] || [ -z "$PLAT_SPEC" ]; then
      build_win386
   fi

   if [ "$PLAT_SPEC" = "win16" ] || [ -z "$PLAT_SPEC" ]; then
      build_win16
   fi

   if [ "$PLAT_SPEC" = "win32" ] || [ -z "$PLAT_SPEC" ]; then
      build_win32
   fi

   if [ "$PLAT_SPEC" = "wasm" ] || [ -z "$PLAT_SPEC" ]; then
      make -f Makefile.wasm DTHRESHOLD=$DEBUG_THRESHOLD DEPTH=VGA BUILD=$BUILD ARCFMT=ZIP pkg_wasm || exit
      make -f Makefile.wasm DTHRESHOLD=$DEBUG_THRESHOLD BUILD=$BUILD ARCFMT=ZIP pkg_wasm || exit
   fi

   if [ "$PLAT_SPEC" = "mac" ] || [ -z "$PLAT_SPEC" ]; then
      build_mac6
   fi

   if [ "$PLAT_SPEC" = "nds" ] || [ -z "$PLAT_SPEC" ]; then
      build_nds
   fi

   echo "All packages built OK!"
}

while [ "$1" ]; do
   case "$1" in
      "-h"|"--help")
         echo "usage: $0 [-r] [prof|run] [platspec]"
         echo
         echo "-r       - Build RELEASE-mode binaries."
         echo "run      - Command: Run build binary for platspec."
         echo "prof     - Command: Build profile data from run."
         echo "platspec - Platform specifier."
         echo
         echo "Current platforms: $PLATFORMS"
         echo
         exit 1
         ;;

      "-r"|"--release")
         BUILD=RELEASE
         DEBUG_THRESHOLD=
         ;;

      "-i"|"--iso")
         PKG_ISO=1
         ;;

      *)
         if [ "prof" = "$1" ]; then
            ACTION=PROFILE
         elif [ "run" = "$1" ]; then
            ACTION=RUN
         elif [[ "$PLATFORMS" == *"$1"* ]]; then
            PLAT_SPEC="$1"
         fi
         ;;
   esac
   shift
done

if [ -z "$PLAT_SPEC" ]; then
   echo "No platform specified for ${ACTION}..."
else
   echo "${ACTION}ing for $PLAT_SPEC..."
fi

if [ "$ACTION" = "PROFILE" ]; then

   if [ "$PLAT_SPEC" = "sdl" ]; then
      echo "Profiling to: prof_output_sdl"
      gprof pkgbuild/dsekai-sdl-*-vga-*-asn/{dsekai,gmon.out} > prof_output_sdl
   else
      echo "Unknown profile target!"
   fi

   exit

elif [ "$ACTION" = "BUILD" ]; then
   do_build

   if [ $PKG_ISO -eq 1 ]; then
      mkisofs -r -J -V "DSEKAI_$GIT_HASH" -o dsekai.iso packages
   fi

elif [ "$ACTION" = "RUN" ]; then
   do_run

fi


