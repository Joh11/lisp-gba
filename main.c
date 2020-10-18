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

int main(void)
{
    const uint32 letter_lambda[2] = {0x10080804, 0x00442828};
    const uint32 selector[2] = {0x810081db, 0xdb810081};

    *REG_DISPCNT = flag_video_mode0 | flag_enable_sprites;

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
	100,  // y coord + other stuff
	100,  // x coord + other stuff
	3 | (0b11 << 0xA) | (0b00 << 0xC), // tile index + others stuff
	0
    };

    // set the selector attributes
    obj_attributes obj_selector = {
	100,  // y coord + other stuff
	50,  // x coord + other stuff
	4 | (0b10 << 0xA) | (0b00 << 0xC), // tile index + others stuff
	0
    };

    
    oam[1] = obj_lambda;
    oam[2] = obj_selector;

    const uint32 delay = 10;
    uint32 num_cycles_last_down = 0;
    
    // Wait forever
    while(1)
    {
	vsync_wait();

	if(is_key_pressed(KEY_UP))
	    obj_selector.attr0 -= 8;
	if(is_key_pressed(KEY_DOWN))
	    obj_selector.attr0 += 8;
	if(is_key_pressed(KEY_LEFT))
	{
	    if(num_cycles_last_down >= delay)
	    {
		num_cycles_last_down = 0;
		obj_selector.attr1 -= 8;
	    }
	    else
		num_cycles_last_down++;
	}
	else if(num_cycles_last_down != 0)
	{
	    num_cycles_last_down = delay;
	}
	
	if(is_key_pressed(KEY_RIGHT))
	    obj_selector.attr1 += 8;
	oam[2] = obj_selector;
    }

    return 0;
}
