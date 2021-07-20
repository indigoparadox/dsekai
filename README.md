
# dsekai

dsekai is an extremely minimal world engine, intended to run on very old hardware. It is originally targeted to the 8086/8088 CPU with the intention to make it more portable later. See below for a list of supported platforms.

There are also some rough examples of CGA graphics programming in graphics/dosg.c, if that's what you're into.

## Rules

Ideally, this engine will compile with legacy compilers as a general rule. For this reason, the following rules/limitation should be observed within the codebase:

* C should be limited to C89/ANSI C in general. No C99 features should be used. In some cases, not all ANSI features should be used.
* Except within platform-specific modules that can definitely handle it, dynamic allocation should also be avoided.
* Except within platform-specific modules that can definitely use them, external dependencies should be avoided.

In general, these restrictions are relaxed for items in the tools/ and check/ subdirectories (though these should also be limited to C89). For this reason, items in the src/ directory should also avoid depending on items in the tools/ and check/ directories (except for in platform-specific modules that do not have these restrictions that may use the tools/ subdirectory. Nothing should depend on the check/ subdirectory).

Items in the tools/ and check/ subdirectories may depend on the src/ directory freely.

## Resources

This engine uses OS-level resources where it can to store assets. On platforms without this facility, assets are converted into headers during compile time and referenced via the same resource-loading mechanisms.

## Platforms

The following platforms are currently supported:

* **MS-DOS w/ CGA Graphics**
* **PalmOS >= 3.5**
* **GNU/Linux with SDL** (May work with other POSIX platforms.)
* **Windows (16-bit)** (And maybe 32-bit.)
* **Mac-OS 6/7** (Coming soon...ish...)

## Compiling

Compiling dsekai for MS-DOS is best achieved with the OpenWatcom compiler (this is what the Makefile uses). Other platforms use their own compilers as outlined in the table below.

Some platforms formerly required Python in order to translate resources from the CGA header format. Tools written in C are now available in the tools/ subdirectory in order to convert resources from more standard (e.g. Windows bitmap) formats into the more efficient formats used by the engine. Please see the Makefile for details.

For MS-DOS, the INCLUDE environment variable must be set to the location of the OpenWatcom header files (e.g. /opt/watcom/h).

## Platforms

The following targets may be built:

| Platform | Make Target         | Requirements |
|----------|---------------------|--------------
| MS-DOS   | bin/dsekai.exe   | [OpenWatcom](https://github.com/open-watcom/open-watcom-v2)
| SDL      | bin/dsekai       | SDL2
| Xlib     | bin/dsekaix      | Xlib
| PalmOS   | bin/dsekai.prc   | [PRCTools](https://github.com/jichu4n/prc-tools-remix)
| Win16    | bin/dsekai16.exe | [OpenWatcom](https://github.com/open-watcom/open-watcom-v2)
| Win32    | bin/dsekai32.exe | [OpenWatcom](https://github.com/open-watcom/open-watcom-v2)
| *MacOS 7 | bin/dsekai16.dsk | [Retro68](https://github.com/autc04/Retro68)
| *NDS     | bin/dsekai.nds   | [DevKitPro](https://devkitpro.org/)

Items marked with * are currently broken.

Just doing "make" will attempt to build all currently working targets.

