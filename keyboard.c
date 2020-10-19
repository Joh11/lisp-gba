#include <stddef.h>

#include "image.h"
#include "keyboard.h"

// -----------------------------------------------------------------------------
// Constants
// -----------------------------------------------------------------------------

static const uint32 big_selector[] = {0x10177, 0x10101, 0x8080ee, 0x808080, 0x1010100, 0x77010100, 0x80808000, 0xe7808000};

// tiles for the keycaps
static const bitpacked_tile4 bg_tile = {0x00000000, 0x00000000};
static const bitpacked_tile4 key_tl = {0x202fe00, 0x2020202};
static const bitpacked_tile4 key_tr = {0x2020202, 0xfe0202};
static const bitpacked_tile4 key_bl = {0x40407f00, 0x40404040};
static const bitpacked_tile4 key_br = {0x40404040, 0x7f4040};

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

    // palette
    bg_palette_mem->data[0] = rbg(0, 0, 0); // transparency color is black
    bg_palette_mem->data[1] = rbg(31, 31, 31); // first color is white
    bg_palette_mem->data[2] = rbg(15, 15, 15); // second color is gray

    // tiles (use charblock 1)
    tile_mem[1][0] = unpack_monochrome_tile4(bg_tile, 1, 0);
    tile_mem[1][1] = unpack_monochrome_tile4(bg_tile, 1, 2);
    tile_mem[1][2] = unpack_monochrome_tile4(key_tl, 2, 0);
    tile_mem[1][3] = unpack_monochrome_tile4(key_tr, 2, 0);
    tile_mem[1][4] = unpack_monochrome_tile4(key_bl, 2, 0);
    tile_mem[1][5] = unpack_monochrome_tile4(key_br, 2, 0);

    // tile map
    fill_keyboard_tilemap(se_mem[0]);
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
