
#ifndef TILEBMPS_H
#define TILEBMPS_H

/*
These are stored as 2bpp 16x16 pixel CGA images in little-endian order.
This makes a huge difference in performance on a real 8086.
*/

const GRAPHICS_TILE gc_tile_field_grass = {
   {0x0, 0x11000000, 0x4000000, 0x4000000, 0x4004, 0x110001, 0x40001, 0x40000, 0x0, 0x0, 0x0, 0x11, 0x4, 0x440004, 0x100000, 0x100000}
};

const GRAPHICS_TILE gc_tile_field_brick_wall = {
   {0x23222322, 0x8b888b88, 0x23222322, 0xfcfcfcfc, 0x22232223, 0x888b888b, 0x22232223, 0xfcfcfcfc, 0x23222322, 0x8b888b88, 0x23222322, 0xfcfcfcfc, 0x22232223, 0x888b888b, 0x22232223, 0xfffcfffc}
};

const GRAPHICS_TILE gc_tile_field_tree = {
   {0x0, 0x545500, 0xdddd01, 0x40777707, 0xc0dddd05, 0x40777707, 0xdddd01, 0x747700, 0xe02e00, 0x800b00, 0x800e00, 0x800b00, 0xa02e00, 0xac203, 0x800, 0x0}
};

#endif /* TILEBMPS_H */
