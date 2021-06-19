#!/usr/bin/env python

import argparse
import re
import logging
import struct

STRUCTS_PATTERN = re.compile( r'const\s*(?P<type>\S*)\s*(?P<name>\S*)\s*=\s*[{\s]*(?P<data>[0-9a-fA-FxX,\s]*)\s*?}' )

X = 0
Y = 1

def row_from_int( row_hex_int, size ):

    logger = logging.getLogger( 'row' )

    new_row = []

    bpp_mask = 0x3
    row_hex_int = struct.unpack( "<I", struct.pack( ">I", row_hex_int ) )[0]

    for bit_idx in range( 0, size[X] ):
        # Grab each pixel from each bit(s).
        int_px = 0
        int_px |= row_hex_int & bpp_mask
        row_hex_int >>= 2
        logger.debug( 'bit_idx %d: int_px: %d', bit_idx, int_px )
        logger.debug( 'adding pixel: %d', int_px )
        new_row.insert( 0, int_px )

    return new_row

def hex_lines_to_grid( hex_lines, size ):

    logger = logging.getLogger( 'hex_lines' )

    grid_out = []

    for y_grid in range( 0, size[Y] ):
        
        logger.debug( 'reading row %d of %d', y_grid, size[Y] )

        # Create a new empty row.
        row_hex_int = int( hex_lines[0], 16 )
        hex_lines.pop( 0 )
        logger.debug( 'read row_hex_int: %d', row_hex_int )
        new_row = row_from_int( row_hex_int, size )

        logger.debug( 'row is %d (Y) (should be %d)',
            len( new_row ), size[X] )
        assert( size[X] == len( new_row ) )

        grid_out.append( new_row )

    return grid_out

def grid_to_hex_lines( grid, size ):
    
    hex_lines_out = []

    for y_grid in range( 0, size[Y] ):
        bits_out = 0
        row_int = 0
        for px in grid[y_grid]:
            row_int <<= 1
            if 3 == px or 2 == px:
                row_int |= 1
            row_int <<= 1
            if 3 == px or 1 == px:
                row_int |= 1
            bits_out += 2
            if size[X] * 2 <= bits_out:
                hex_lines_out.append( "0x%x" % row_int )

    return hex_lines_out
    
def main():

    parser = argparse.ArgumentParser()

    parser.add_argument( '-if', '--infile' )
    parser.add_argument( '-of', '--outfile' )
    parser.add_argument( '-s', '--size', nargs='+', type=int )

    args = parser.parse_args()

    assert( args.infile != args.outfile )

    size = tuple( args.size )

    with open( args.outfile, 'w' ) as out_file:
        with open( args.infile, 'r' ) as in_file:
            in_text = in_file.read()
            in_structs = STRUCTS_PATTERN.findall(
                in_text, re.MULTILINE | re.DOTALL )

            # Copy the prelude and hold the postlude for later.
            postlude = ''
            in_prelude = True
            for line in in_text.split( '\n' ):
                if len( line ) == 0:
                    continue
                elif '#' == line[0] and in_prelude:
                    out_file.write( line + '\n' )
                elif '#' == line[0] and not in_prelude:
                    postlude += line + '\n'
                else:
                    in_prelude = False

            for match in in_structs:
                grid = hex_lines_to_grid( match[2].split( ', ' ), size )
                out_lines = grid_to_hex_lines( grid, size )
                out_file.write(
                    'const %s %s = {{%s}};\n' % \
                    (match[0], match[1], ', '.join( out_lines )) )

            out_file.write( postlude )

if '__main__' == __name__:
    main()

