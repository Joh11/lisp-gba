#include <stddef.h>

#include "image.h"
#include "keyboard.h"

// -----------------------------------------------------------------------------
// Constants
// -----------------------------------------------------------------------------

static const uint32 big_selector[] = {0x10177, 0x10101, 0x8080ee, 0x808080, 0x1010100, 0x77010100, 0x80808000, 0xe7808000};


// -----------------------------------------------------------------------------
// Local variables
// -----------------------------------------------------------------------------

static obj_attributes obj_selector;

// -----------------------------------------------------------------------------
// Functions
// -----------------------------------------------------------------------------

void init_keyboard()
{
    // -------------------------------------------------------------------------
    // Init sprites
    // -------------------------------------------------------------------------

    // palette
    tile_palette_mem->data[0] = rbg(31, 0, 31); // transparency color is magenta
    tile_palette_mem->data[1] = rbg(31, 31, 31); // first color is white
    tile_palette_mem->data[2] = rbg(31, 0, 0); // third color is red

    // tiles
    unpack_monochrome_tiles(big_selector, &tile_mem[4][1], 4, 0x2, 0x0);

    // sprite
    obj_selector.attr0 = 96 // y coord
	| (0b00 << 0xe); // sprite shape (16x16)
    obj_selector.attr1 = 32 // x coord
	| (0b01 << 0xe); // sprite size (16x16)
    obj_selector.attr2 = 1 // tile index
	| (0b10 << 0xA) // priority
	| (0b00 << 0xC); // palette-bank

    oam[0] = obj_selector;
    
    // -------------------------------------------------------------------------
    // Init backgrounds
    // -------------------------------------------------------------------------
}

void fill_keyboard_tilemap(volatile screenblock sb)
{
    for(size_t row = 0 ; row < 12 ; ++row)
    {
	for(size_t col = 0 ; col < 32 ; ++col)
	    sb[32 * row + col] = 0;
    }
    for(size_t row = 12 ; row < 20 ; ++row)
    {
	for(size_t col = 0 ; col < 32 ; ++col)
	{
	    if ((col < 4) || (col > 25))
	    {
		sb[32 * row + col] = 0;
		continue;
	    }
	    sb[32 * row + col] = (row % 2) ?
		3 + 2 * (col % 2)
		: 2 + 2 * (col % 2);
	}
    }
    for(size_t row = 20 ; row < 32 ; ++row)
    {
	for(size_t col = 0 ; col < 32 ; ++col)
	    sb[32 * row + col] = 0;
    }
}

void update_selector()
{
    oam[0] = obj_selector;
}

void move_cursor(uint32 dx, uint32 dy)
{
    obj_selector.attr0 += dy;
    obj_selector.attr1 += dx;
}
