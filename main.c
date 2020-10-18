#include <stdlib.h>

#include "constants.h"
#include "input.h"

#define RED   0x001F
#define GREEN 0x03E0
#define BLUE  0x7C00

static inline void vsync_wait()
{
    while(REG_VCOUNT >= 160); // wait till VDraw
    while(REG_VCOUNT < 160);  // wait till VBlank
}

static inline uint16 rbg(int r, int g, int b)
{
    return (b << 10) | (g << 5) | r;
}

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

int main(void)
{
    const uint32 letter_lambda[2] = {0x10080804, 0x00442828};
    const uint32 selector[2] = {0x810081db, 0xdb810081};

    REG_DISPCNT = flag_video_mode0 | flag_enable_bg0 | flag_enable_sprites;

    // sprite 0 of reg 5 will be totally white (the 2nd color of the palette)
    tile4 white_sprite = {
	{0x11111111, 0x22222222, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111},
    };
    tile_mem[5][0] = white_sprite;
    tile_mem[4][1] = white_sprite;
    tile_mem[4][2] = white_sprite;

    tile_mem[4][3] = unpack_monochrome_tile4(letter_lambda, 0x1, 0x0);
    tile_mem[4][4] = unpack_monochrome_tile4(selector, 0x2, 0x0);
    
    
    // set the palette
    tile_palette_mem->data[0] = rbg(0, 0, 31); // transparency color is black
    tile_palette_mem->data[1] = rbg(31, 31, 31); // first color is white
    tile_palette_mem->data[2] = rbg(31, 0, 0); // third color is red

    // set the lambda attributes
    obj_attributes obj_lambda = {
	160/2 + 16 + 4,  // y coord + other stuff
	240/2 - 4,  // x coord + other stuff
	3 | (0b11 << 0xA) | (0b00 << 0xC), // tile index + others stuff
	0
    };

    // set the selector attributes
    obj_attributes obj_selector = {
	160 / 2,  // y coord + other stuff
	0,  // x coord + other stuff
	4 | (0b10 << 0xA) | (0b00 << 0xC), // tile index + others stuff
	0
    };

    
    oam[1] = obj_lambda;
    oam[2] = obj_selector;

    const uint32 delay = 10;
    uint32 num_cycles_last_down = 0;


    // -----------------------------------------------------------------------------
    // Background
    // -----------------------------------------------------------------------------

    // Using BG0 for the keyboard
    REG_BG0CNT = 3 // low priority
	| (1 << 2) // charblock
	| (0 << 6) // mosaic
	| (0 << 7) // 4bpp
	| (0 << 8) // screenblock
	| (0 << 0xd) // affine wrapping (dont care)
	| (0b00 << 0xe) // 32x32 tiles (we need at least 20 tiles wide)
	;

    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;

    // Use screenblock 0
    fill_keyboard_tilemap(se_mem[0]);

    // Fill the tiles
    // Use the charblock 1
    const bitpacked_tile4 black_tile = {0x00000000, 0x00000000};;
    tile4 white_tile = {
	{0x11111111, 0x11111111, 0x11111111, 0x11111111,
	 0x11111111, 0x11111111, 0x11111111, 0x11111111},
    };
    tile4 gray_tile = {
	{0x11111111, 0x12222221, 0x12222221, 0x12222221,
	 0x12222221, 0x12222221, 0x12222221, 0x11111111},
    };

    // tiles for the keycaps
    const bitpacked_tile4 key_tl = {0x202fe00, 0x2020202};
    const bitpacked_tile4 key_tr = {0x2020202, 0xfe0202};
    const bitpacked_tile4 key_bl = {0x40407f00, 0x40404040};
    const bitpacked_tile4 key_br = {0x40404040, 0x7f4040};
    
    tile_mem[1][0] = unpack_monochrome_tile4(black_tile, 1, 0);
    tile_mem[1][1] = gray_tile;
    tile_mem[1][2] = unpack_monochrome_tile4(key_tl, 2, 0);
    tile_mem[1][3] = unpack_monochrome_tile4(key_tr, 2, 0);
    tile_mem[1][4] = unpack_monochrome_tile4(key_bl, 2, 0);
    tile_mem[1][5] = unpack_monochrome_tile4(key_br, 2, 0);

    // Set the palette
    bg_palette_mem->data[0] = rbg(0, 0, 0); // transparency color is black
    bg_palette_mem->data[1] = rbg(31, 31, 31); // first color is white
    bg_palette_mem->data[2] = rbg(15, 15, 15); // second color is gray
    
    // Wait forever
    while(1)
    {
	vsync_wait();

	if(key_pressed_delay(ENUM_UP))
	    obj_selector.attr0 -= 16;
	if(key_pressed_delay(ENUM_DOWN))
	    obj_selector.attr0 += 16;
	if(key_pressed_delay(ENUM_LEFT))
	    obj_selector.attr1 -= 16;
	if(key_pressed_delay(ENUM_RIGHT))
	    obj_selector.attr1 += 16;
	
	oam[2] = obj_selector;
	
	update_key_state();

    }

    return 0;
}
