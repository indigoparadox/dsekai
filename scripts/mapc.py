#!/usr/bin/env python3

import argparse
import logging
import json
import os

def main():

    parser = argparse.ArgumentParser()

    parser.add_argument( '-j', '--json', action='store' )
    parser.add_argument( '-o', '--out', action='store' )

    args = parser.parse_args()

    with open( args.json, 'r' ) as json_file:
        json_data = json.loads( json_file.read() )
        with open( args.out, 'w' ) as h_file:
            map_basename = os.path.basename( args.json ).split( '.' )[0]
            height = json_data['height']
            width = json_data['width']
            assert( width == 30 )
            assert( height == 30 )
            tile_out = 0

            h_file.write( 'const struct TILEMAP gc_%s = {\n' % map_basename )
            h_file.write( '   "%s",\n' % map_basename )
            h_file.write( '   { 0 },\n' )
            h_file.write( '   {\n      ' )
            tile_idx = 0
            for tile in json_data['layers'][0]['data']:
                tile = int( tile )
                if 0 < tile:
                    tile -= 1
                if 0 == tile_idx % 2:
                    tile_out |= (tile & 0x0f)
                else:
                    tile_out <<= 4
                    tile_out |= (tile & 0x0f)
                    h_file.write( '0x%02x, ' % tile_out )
                    tile_out = 0
                if 0 == tile_idx % width and 0 != tile_idx:
                    h_file.write( '\n      ' )
                    
                tile_idx += 1
            assert( 900 == tile_idx )
            h_file.write( '\n   },\n   {\n      ' )
            for tile_flags in \
            range( 0, int( len( json_data['layers'][0]['data'] ) ) ):
                h_file.write( '0x00, ' )
            h_file.write( '\n   }\n' )
            #uint8_t tiles[(TILEMAP_TH * TILEMAP_TW) / 2];
            #uint8_t tiles_flags[TILEMAP_TH * TILEMAP_TW];
            h_file.write( '};\n' )
            

if '__main__' == __name__:
    main()

