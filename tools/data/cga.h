
#ifndef CGA_H
#define CGA_H

#define CGA_HEADER_SZ                     30

#define CGA_HEADER_OFFSET_VERSION         2
#define CGA_HEADER_OFFSET_WIDTH           4
#define CGA_HEADER_OFFSET_HEIGHT          6
#define CGA_HEADER_OFFSET_BPP             8
#define CGA_HEADER_OFFSET_PLANE1_OFFSET   10
#define CGA_HEADER_OFFSET_PLANE1_SZ       12
#define CGA_HEADER_OFFSET_PLANE2_OFFSET   14
#define CGA_HEADER_OFFSET_PLANE2_SZ       16
#define CGA_HEADER_OFFSET_PALETTE         18
#define CGA_HEADER_OFFSET_ENDIAN          20
#define CGA_HEADER_OFFSET_RESERVED_1      22
#define CGA_HEADER_OFFSET_RESERVED_2      24
#define CGA_HEADER_OFFSET_RESERVED_3      26
#define CGA_HEADER_OFFSET_RESERVED_4      28

struct CGA_HEADER {
   uint16_t id;
   uint16_t version;
   uint16_t width;
   uint16_t height;
   uint16_t bpp;
   uint16_t plane1_offset;
   uint16_t plane1_sz;
   uint16_t plane2_offset;
   uint16_t plane2_sz;
   uint16_t palette;
   uint16_t endian;
   uint16_t reserved1;
   uint16_t reserved2;
   uint16_t reserved3;
   uint16_t reserved4;
};

#ifndef NO_CGA_FUNCTIONS

struct CONVERT_GRID;
struct CONVERT_OPTIONS;

uint32_t cga_write_plane(
   uint8_t*, uint32_t, const struct CONVERT_GRID* grid, int, int );
int cga_write_file(
   const char*, const struct CONVERT_GRID*, struct CONVERT_OPTIONS* );
int cga_write(
   uint8_t*, uint32_t, const struct CONVERT_GRID*, struct CONVERT_OPTIONS* );
struct CONVERT_GRID* cga_read_file(
   const char* path, struct CONVERT_OPTIONS* o );
struct CONVERT_GRID* cga_read(
   const uint8_t*, uint32_t, struct CONVERT_OPTIONS* );

#endif /* NO_CGA_FUNCTIONS */

#endif /* CGA_H */

