
# dsekai

[![Code Grade](https://api.codiga.io/project/29729/score/svg)](https://www.codiga.io)

* [dsekai](#deskai)
  * [Roadmap](#roadmap)
  * [Documentation](#documentation)
* [Building](#building)
  * [Datafiles](#datafiles)
  * [Platforms](#platforms)
* [Modding](#modding)
  * [Mapping](#mapping)
  * [Skinning](#skinning)
  * [Scripting](#scripting)
* [Tools](#tools)
  * [mkresh](#mkresh)
  * [convert](#convert)
  * [map2asn](#map2asn)
  * [headpack](#headpack)

![Overworld Screenshot](screens/overwrld.png)

dsekai is an extremely minimal world engine, intended to run on very old hardware. It is originally targeted to the 8086/8088 CPU with the intention to make it more portable later. See below for a list of supported platforms.

This project is being built around/in tandem with the unilayer project, in order to create a somewhat-universal framework/library for games on legacy/weird platforms.

There are also some rough examples of CGA graphics programming in unilayer/graphics/dosg.c, if that's what you're into.

## Roadmap

Features that are planned include:

* ASN.1 save dumps.
* Housing system based on in-game map editing.
* Item usage and crafting.
* Minimal resource use (shooting for <500k RAM).
* Multi-height raycasting POV support.
* Compile script source files as part of JSON loading.

Ideally, this engine will compile with legacy compilers as a general rule. For this reason, the following rules/limitations should be observed within the codebase:

* C should be limited to C89/ANSI C in general. No C99 features should be used. In some cases, not all ANSI features should be used.
* Except within platform-specific modules that can definitely handle it, dynamic allocation should also be avoided.
* Except within platform-specific modules that can definitely use them, external dependencies should be avoided.

In general, these restrictions are relaxed for items in the tools/ and check/ subdirectories (though these should also be limited to C89). For this reason, items in the src/ directory should also avoid depending on items in the tools/ and check/ directories (except for in platform-specific modules that do not have these restrictions that may use the tools/ subdirectory. Nothing should depend on the check/ subdirectory).

Items in the tools/ and check/ subdirectories may depend on the src/ directory freely.

## Documentation

Some attempt has been made at documentation with [doxygen](https://www.doxygen.nl/index.html). Simply run doxygen in the project root in order to generate HTML documentation under doc/.

This documentation is also available at [https://indigoparadox.github.io/dsekai/](https://indigoparadox.github.io/dsekai/).

This is a work in progress.

# Building

Currently, this project uses GNU make to (cross-)build on a GNU/Linux system. Compilers and tools required depend on the desired [platforms](#platforms) to build for.

For MS-DOS, the INCLUDE environment variable must be set to the location of the OpenWatcom header files (e.g. /opt/watcom/h).

The script pkgs.sh in the root directory should build the recommended set of packages if the proper compilers are installed.

## Datafiles

This engine should be capable of retrieving assets (levels, graphics, etc) from different locations, depending on how it was built (e.g. ASN.1 vs JSON maps support). Aside from loading directly from the filesystem, it should be able to use OS-level resources embedded in the binary. On platforms without this facility, a rough approximation of this is emulated using [object embedding](headpack).

## Platforms

The following platforms are currently supported:

* **MS-DOS w/ CGA Graphics** (Maybe VGA graphics in the future...)
* **GNU/Linux with SDL** (May work with other POSIX platforms.)
* **Windows (16-bit/32-bit)**
* **WebASM**

The following platforms are planned to be supported but not yet functional:

* **PalmOS >= 3.5**
* **Mac-OS 6/7**
* **GNU/Linux with Xlib** (May work with other POSIX platforms.)
* **GNU/Linux with NCurses** (May work with other POSIX platforms.)
* **Nintendo DS**

| Platform | Make Target         | Requirements |
|----------|---------------------|--------------
| +MS-DOS  | bin/dsekai.exe   | [OpenWatcom](https://github.com/open-watcom/open-watcom-v2)
| SDL      | bin/dsekai       | SDL2
| Xlib     | bin/dsekaix      | Xlib
| \*PalmOS  | bin/dsekai.prc   | [PRCTools](https://github.com/jichu4n/prc-tools-remix)
| +Win16   | bin/dsekai16.exe | [OpenWatcom](https://github.com/open-watcom/open-watcom-v2)
| Win32    | bin/dsekai32.exe | [OpenWatcom](https://github.com/open-watcom/open-watcom-v2)
| \*MacOS 7 | bin/dsekai16.dsk | [Retro68](https://github.com/autc04/Retro68)
| \*NDS     | bin/dsekai.nds   | [DevKitPro](https://devkitpro.org/)
| WebASM    | bin/dsekai.js    | [emscripten](https://github.com/emscripten-core/emscripten)
| \*Curses  | bin/dsekait      | NCurses

 * + Platforms marked with + do not support JSON tilemaps directly.
 * \* Platforms marked with \* are currently broken.

Just using "make" will attempt to build all currently working targets with OS-level resources and statically compiled maps.

## Platform-Specific Notes

### MS-DOS

dsekai is being written to support MS-DOS in real mode on an 8086. This means that it only uses conventional memory. This leads to some technical considerations:

* dsekai uses overlays to page unused engines to disk during execution. The engine .ovl files must be kept with the dsekai.exe file for it to work properly! Please see the Makefile.dos for more information.

# Mechanics

## Items

Items are defined in each tilemap in an array. Through scripts, items can be
coped from the item definition array into the player's inventory where they
can then be used even if the player is on a different tilemap.

## Crops

Crops are also defined in each tilemap in an array. However, seeds in the
player's inventory only reference the crop GID. This means that crops can
only grow on a tilemap in which they are defined. This also means that crop
GIDs MUST NOT collide between tilemaps.

## Crops

# Modding

## Mapping

Maps are stored as JSON files in the format used by the [Tiled](https://github.com/mapeditor/tiled) map editor.

Please see map_field.json in the assets directory for an example.

The following special considerations should be observed for the dsekai engine:

 * [Scripts](#scripting) are loaded from the map file according to the rules in that section.
 * NPC (or "mobile") spawns are stored in an objects layer called "objects". The engine selects the NPC sprite based on its name and its script based on a custom property called "script".
 * The static layer of the map is loaded from a layer called "terrain".

## Skinning

TBA

## Scripting

Information on scripting including detailed instruction reference has been [moved into the documentation](https://indigoparadox.github.io/dsekai/group__scripting__instructions__sect.html).

For the time being, the script compiler can be compiled by using ``make bin/compile``, and then running ``bin/compile <path-to-script.s>`` to generate bytecode output that can be pasted into tilemaps.

See ``assets/m\_field`` for some example scripts.

# Tools

Included in the tools/ subdirectory are a few tools required for preprocessing and compiling. These include tools for preprocessing resources from easier-to-edit formats to more compact formats for resource-limited platforms, as well as tools for packing up resources as part of compilation.

## mkresh

Creates header files with resource listings to be included as OS-level resources.

## convert

Converts between various image formats common to resource-limited platforms.

### Usage

./bin/convert [options] -ic \<in_fmt\> -oc \<out_fmt\> -if \<in_file\> -of \<out_file\>

### Options:

 * -ic \[In format extension from Input Formats table.\]
 * -oc \[Out format extension from Output Formats table.\]

### CGA options:

These options only apply to raw CGA files:

 * -ib \[in bpp\] \(defaults to 2\)
 * -ob \[out bpp\] \(defaults to input bpp\)
 * -iw \[in width\] \(requried for CGA in\)
 * -ih \[in height\] \(required for CGA in\)
 * -il \[in line padding\] (full-screen uses 192\)
 * -ol \[out line padding\]

### Input Formats

| Type            | Extension |
|-----------------|-----------|
| Windows Bitmap  | bmp       |
| Mac Icon        | icns      |
| CGA Memory Dump | cga       |

### Output Formats

| Type            | Extension |
|-----------------|-----------|
| Windows Bitmap  | bmp       |
| Mac Icon        | icns      |
| CGA Memory Dump | cga       |

## map2asn

Converts maps from JSON to a more compact ASN.1 representation for use with resource-limited platforms.

Usage: map2asn \<JSON tilemap input path\> \<ASN.1 tilemap output path\>

## headpack

Packs resources into #include-able header files to be inserted directly into the final binary.

Usage: headpack \<header path\> \<paths to files to encode\>

