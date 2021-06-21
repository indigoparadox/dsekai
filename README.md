
# dsekai

dsekai is an extremely minimal world engine, intended to run on very old hardware. It is originally targeted to the 8086/8088 CPU with the intention to make it more portable later. See below for a list of supported platforms.

There are also some rough examples of CGA graphics programming in graphics/dosg.c, if that's what you're into.

## Platforms

The following platforms are currently supported:

* **MS-DOS**
* **PalmOS >= 3.5**
* **GNU/Linux with SDL** (May work with others.)
* **Windows (16-bit)** (Coming soon.)

## Compiling

Compiling dsekai for MS-DOS is best achieved with the OpenWatcom compiler (this is what the Makefile uses).

Some targets require python in order to run the script that translates graphic resources from CGA header files to bitmaps for the target platform. Please note that this means this cannot be compiled directly on those platforms, and must be cross-compiled (preferably using OpenWatcom, as noted above).

The INCLUDE environment variable must be set to the location of the OpenWatcom header files (e.g. /opt/watcom/h). Then the following targets may be built:

| Platform | Make Target         | Requirements |
|----------|---------------------|--------------
| MS-DOS   | bin/dsekai.exe   | [OpenWatcom](https://github.com/open-watcom/open-watcom-v2)
| SDL      | bin/dsekai       | SDL2
| PalmOS   | bin/dsekai.prc   | Python, [PRCTools](https://github.com/jichu4n/prc-tools-remix)
| Win16    | bin/dsekai16.exe | Python, [OpenWatcom](https://github.com/open-watcom/open-watcom-v2)
