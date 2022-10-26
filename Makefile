
CALLER=MASTER
# Build file resources by default, otherwise use win/palm resources.
ifneq ($(RESOURCE),DEFAULT)
   RESOURCE=FILE
ifneq ($(FMT_JSON),TRUE)
   FMT_ASN=TRUE
endif
endif

# Figure out what packages we can build based on platform limitations.

BUILD_PKG_ALL := pkg_sdl pkg_win16 pkg_win32

# Palm not included by default as it requires MONO and RESOURCE=DEFAULT.
BUILD_PALM=0
# DOS not included by default as it requires CGA to be set.
BUILD_DOS=0
# xlib not included by default as it requires CGA to be set.
BUILD_XLIB=0
# WASM not included as it requires RESOURCE=DEFAULT.
BUILD_WASM=0

ifeq ($(DEPTH),MONO)
   ifeq ($(RESOURCE),DEFAULT)
      BUILD_PALM=1
      BUILD_PKG_ALL += pkg_palm
   endif
else ifeq ($(DEPTH),VGA)
   ifeq ($(RESOURCE),DEFAULT)
      BUILD_WASM=1
      BUILD_PKG_ALL += pkg_wasm
   endif
else
   # DEPTH=VGA
   BUILD_DOS=1
   BUILD_PKG_ALL += pkg_dos
   BUILD_XLIB=1
   BUILD_PKG_ALL += pkg_xlib
   ifeq ($(RESOURCE),DEFAULT)
      BUILD_WASM=1
      BUILD_PKG_ALL += pkg_wasm
   endif
endif

.PHONY: clean $(BUILD_PKG_ALL)

all: $(BUILD_PKG_ALL)

# Include platform-specific Makefiles.

include Makefile.inc

include Makefile.sdl
include Makefile.win16
include Makefile.win32

ifeq ($(BUILD_PALM),1)
   include Makefile.palm
endif
ifeq ($(BUILD_DOS),1)
   include Makefile.dos
endif
ifeq ($(BUILD_XLIB),1)
   include Makefile.xlib
endif
ifeq ($(BUILD_WASM),1)
   include Makefile.wasm
endif

# NDS makefile not currently included as it is broken on our system.

# ====== Clean ======

clean:
	rm -rf data obj obj-* bin bin-* gen gen-* dep dep-* *.err .rsrc .finf gmon.out log*.txt packages pkgbuild dsdos prof_output* unimake test_unilayer dsekai.iso

