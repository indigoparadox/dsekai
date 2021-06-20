#!/usr/bin/env python

import argparse
import re
import logging
import struct
import os

STRUCTS_PATTERN = re.compile( r'const\s*(?P<type>\S*)\s*(?P<name>\S*)\s*=\s*[{\s]*(?P<data>[0-9a-fA-FxX,\s]*)\s*?}' )

X = 0
Y = 1

def switch_endian( int_out, size, bpp ):
    if 8 == size[X] * bpp:
        int_out = struct.unpack(
            "<B", struct.pack( ">B", int_out ) )[0]
    elif 16 == size[X] * bpp:
        int_out = struct.unpack(
            "<H", struct.pack( ">H", int_out ) )[0]
    elif 32 == size[X] * bpp:
        int_out = struct.unpack(
            "<I", struct.pack( ">I", int_out ) )[0]
    else:
        raise Exception( 'unpackable size specified' )
    return int_out

def row_from_int( row_hex_int, size, bpp, endian ):

    logger = logging.getLogger( 'row' )

    new_row = []

    bpp_mask = 0x1
    for bpp_shift in range( 0, bpp - 1 ):
        bpp_mask <<= 1
        bpp_mask |= 1

    if 2 == bpp:
        assert( 3 == bpp_mask )
    elif 1 == bpp:
        assert( 1 == bpp_mask )

    if 'l' == endian:
        row_hex_int = switch_endian( row_hex_int, size, bpp )

    for bit_idx in range( 0, size[X] ):
        # Grab each pixel from each bit(s).
        int_px = 0
        int_px |= row_hex_int & bpp_mask
        row_hex_int >>= bpp
        logger.debug( 'bit_idx %d: int_px: %d', bit_idx, int_px )
        logger.debug( 'adding pixel: %d', int_px )
        new_row.insert( 0, int_px )

    return new_row

def hex_lines_to_grid( hex_lines, size, bpp, endian ):

    logger = logging.getLogger( 'hex_lines' )

    grid_out = []

    for y_grid in range( 0, size[Y] ):
        
        logger.debug( 'reading row %d of %d', y_grid, size[Y] )

        # Create a new empty row.
        row_hex_int = int( hex_lines[0], 16 )
        hex_lines.pop( 0 )
        logger.debug( 'read row_hex_int: %d', row_hex_int )
        new_row = row_from_int( row_hex_int, size, bpp, endian )

        logger.debug( 'row is %d (Y) (should be %d)',
            len( new_row ), size[X] )
        assert( size[X] == len( new_row ) )

        grid_out.append( new_row )

    return grid_out

def grid_to_hex_lines( grid, size, bpp ):
    
    hex_lines_out = []

    for y_grid in range( 0, size[Y] ):
        bits_out = 0
        row_int = 0
        for px in grid[y_grid]:
            if 1 < bpp:
                row_int <<= 1
                if 3 == px or 2 == px:
                    row_int |= 1
            row_int <<= 1
            if (1 < bpp and 3 == px) or 1 == px:
                row_int |= 1
            bits_out += bpp
            if size[X] * bpp <= bits_out:
                hex_lines_out.append( "0x%x" % row_int )

    return hex_lines_out

def convert_to_header( out_path, grids, size, bpp, text='' ):

    with open( out_path, 'w' ) as out_file:
        # Copy the prelude and hold the postlude for later.
        postlude = ''
        in_prelude = True
        for line in text.split( '\n' ):
            if len( line ) == 0:
                continue
            elif '#' == line[0] and in_prelude:
                out_file.write( line + '\n' )
            elif '#' == line[0] and not in_prelude:
                postlude += line + '\n'
            else:
                in_prelude = False

        for new_grid in grids:
            out_lines = grid_to_hex_lines( new_grid[2], size, bpp )
            out_file.write(
                'const %s %s = {{%s}};\n' % \
                (new_grid[0], new_grid[1], ', '.join( out_lines )) )

        out_file.write( postlude )

def convert_to_bmp( out_path, grids, size, bpp, text='' ):
    
    logger = logging.getLogger( 'cvtbmp' )

    for grid in grids:
        bmp_path = os.path.join( out_path, grid[1] + '.bmp' )
        with open( bmp_path, 'wb' ) \
        as out_file:

            out_file.write( b'BM' )
            offset_sz = out_file.tell()
            out_file.write( struct.pack( '<IHH', 0, 0, 0 ) )
            offset_pxo = out_file.tell()
            out_file.write( struct.pack( '<I', 0 ) )

            # Image header.
            out_file.write( struct.pack( '<IiiHHIIiiII',
                40,
                size[X],
                size[Y],
                1,
                1,
                0,
                0,
                72,
                72,
                0,
                0 ) )

            # Palette.
            out_file.write( struct.pack( 'BBBB', 255, 255, 255, 0 ) ) # White
            out_file.write( struct.pack( 'BBBB', 0, 0, 0, 0 ) ) # Black

            offset_pxd = out_file.tell()

            row_idx = 0
            for row in reversed( grid[2] ):
                row_byte = 0
                logger.debug(
                    'printing row %d (%d pixels)', row_idx, len( row ) )
                px_idx = 0
                for px in row:
                    row_byte <<= 1
                    row_byte |= 0 if 0 == px else 1
                    if 7 <= px_idx:
                        logger.debug( 'writing byte: %x', row_byte )
                        out_file.write( struct.pack( 'B', row_byte ) )
                        row_byte = 0
                        px_idx = 0
                    else:
                        px_idx += 1

                # Write padding so each row is 4 bytes.
                for padding_idx in range( 0, 4 - bpp ):
                    out_file.write( b'\x00' )

                row_idx += 1

            offset_end = out_file.tell()

            # Go back and write bitmap size field.
            out_file.seek( offset_sz )
            out_file.write( struct.pack( '<I', offset_end ) )

            # Go back and write pixel data start field.
            out_file.seek( offset_pxo )
            out_file.write( struct.pack( '<I', offset_pxd ) )

            yield (grid[0], grid[1], bmp_path)

def main():

    parser = argparse.ArgumentParser()

    parser.add_argument( '-if', '--infiles', nargs='+' )
    parser.add_argument( '-of', '--outfile' )
    parser.add_argument( '-bi', '--bppin', type=int )
    parser.add_argument( '-bo', '--bppout', type=int )
    parser.add_argument( '-ei', '--endianin' )
    parser.add_argument( '-c', '--convert', type=str )
    parser.add_argument( '-s', '--size', nargs='+', type=int )
    parser.add_argument( '-v', '--verbose', action='store_true' )
    parser.add_argument( '-r', '--resources' )
    parser.add_argument( '-ri', '--resids' )
    parser.add_argument( '-rc', '--resconsts' )
    parser.add_argument( '-re', '--externs' )

    args = parser.parse_args()

    print( args )

    if args.verbose:
        logging.basicConfig( level=logging.DEBUG )
    else:
        logging.basicConfig( level=logging.WARNING )

    size = tuple( args.size )
    bppout = args.bppout
    if not args.bppout:
        bppout = args.bppin

    resources = []

    for infile in args.infiles:
        with open( infile, 'r' ) as in_file:
            in_text = in_file.read()
            in_structs = STRUCTS_PATTERN.findall(
                in_text, re.MULTILINE | re.DOTALL )

            grids = []
            for match in in_structs:
                new_grid = \
                    hex_lines_to_grid( match[2].split( ', ' ), size, args.bppin,
                    args.endianin )
                grids.append( (match[0], match[1], new_grid) )

            if 'header' == args.convert:
                convert_to_header( args.outfile, grids, size, bppout, in_text )
            elif 'bitmap' == args.convert:
                for bmp_path_out in \
                convert_to_bmp( args.outfile, grids, size, bppout, in_text ):
                    resources.append( bmp_path_out )
            elif 'none' == args.convert:
                for grid in grids:
                    resources.append( (grid[0], grid[1]) )

    if args.resources:
        with open( args.resources, 'w' ) as res_file:
            for res in resources:
                res_file.write( 'BITMAP ID {}_id "{}"\n'.format(
                    res[1], res[2] ) )

    if args.resids:
        with open( args.resids, 'w' ) as res_id_file:
            res_id_next = 5000
            for res in resources:
                res_id_file.write( '#define {}_id {}\n'.format(
                    res[1], res_id_next ) )
                res_id_next += 1

    if args.resconsts:
        with open( args.resconsts, 'w' ) as res_c_file:
            for res in resources:
                res_c_file.write( 'const {} {} = {}_id;\n'.format(
                    res[0], res[1], res[1] ) )

    if args.externs:
        with open( args.externs, 'w' ) as extern_file:
            for res in resources:
                extern_file.write(
                    'extern const {} {};\n'.format( res[0], res[1] ) )

if '__main__' == __name__:
    main()

