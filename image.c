#include <stddef.h>

#include "image.h"

// Takes a bitpacked monochrome tile, and allocate a new 8x8 pixels
// tile (4bpp) from it.
tile4 unpack_monochrome_tile4(const uint32 data[2], uint8 fg_index, uint8 gb_index)
{
    tile4 ret = {{0, 0, 0, 0, 0, 0, 0, 0},};

    // 4 first lines
    for(uint32 line = 0; line < 4 ; ++line)
    {
	// Takes 8 bits
	uint32 line_bits = (data[0] >> (8 * line)) & 0xff;
	// Map onto 32 bits
	uint32 color = 0;
	// Take the bit at the ith position
	for (uint32 i = 0 ; i < 8 ; ++i)
	{
	    uint32 bit = (line_bits >> i) & 1;
	    uint8 index = bit? fg_index : gb_index;
	    color = color | (index << (4 * i));
	}
	ret.data[line] = color;
    }

    // 4 last lines
    for(uint32 line = 0; line < 4 ; ++line)
    {
	// Takes 8 bits
	uint32 line_bits = (data[1] >> (8 * line)) & 0xff;
	// Map onto 32 bits
	uint32 color = 0;
	// Take the bit at the ith position
	for (uint32 i = 0 ; i < 8 ; ++i)
	{
	    uint32 bit = (line_bits >> i) & 1;
	    uint8 index = bit? fg_index : gb_index;
	    color = color | (index << (4 * i));
	}
	ret.data[line + 4] = color;
    }
    
    return ret;
}

void unpack_monochrome_tiles(const uint32* data, tile4* dest, uint32 num_tiles, uint8 fg_index, uint8 gb_index)
{
    for(size_t ntile = 0 ; ntile < num_tiles ; ++ntile)
	dest[ntile] = unpack_monochrome_tile4(&data[2*ntile], fg_index, gb_index);
}
