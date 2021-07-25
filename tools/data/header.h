
#ifndef HEADER_H
#define HEADER_H

struct CONVERT_GRID;
struct CONVERT_OPTIONS;

int header_img_write_file(
   const char*, const struct CONVERT_GRID*, struct CONVERT_OPTIONS* );
int header_img_write(
   uint8_t*, uint32_t, const struct CONVERT_GRID*, struct CONVERT_OPTIONS* );

#endif /* HEADER_H */

