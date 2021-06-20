
#ifndef ENGINES_H
#define ENGINES_H

int topdown_loop(
   struct MOBILE mobiles[MOBILES_MAX], int* mobiles_count,
   uint8_t (*tiles_flags)[TILEMAP_TH][TILEMAP_TW] );

#endif /* ENGINES_H */

