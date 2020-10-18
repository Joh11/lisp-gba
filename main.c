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
    // Test of a letter
    /*
      ..#.....
      ...#....
      ...#....
      ....#...
      ...#.#..
      ...#.#..
      ..#...#.
      ........
    */
    /* bytes:
       0x04 0x08 0x08 0x0F 
       0x28 0x28 0x44 0x00
     */
    //
    const uint32 letter_lambda[2] = {0x10080804, 0x00442828};

    *REG_DISPCNT = flag_video_mode0 | flag_enable_sprites;

    // sprite 0 of reg 5 will be totally white (the 2nd color of the palette)
    tile4 white_sprite = {
	{0x11111111, 0x22222222, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111},
    };
    tile_mem[5][0] = white_sprite;
    tile_mem[4][1] = white_sprite;
    tile_mem[4][2] = white_sprite;

    const tile4 lambda_sprite = {
	{0x00000100, 0x00001000, 0x00001000, 0x00010000, 0x00101000, 0x00101000, 0x01000100},};
    tile_mem[4][3] = unpack_monochrome_tile4(letter_lambda, 0x1, 0x0);
    
    // set the palette
    tile_palette_mem->data[0] = rbg(0, 0, 31); // transparency color is black
    tile_palette_mem->data[1] = rbg(31, 31, 31); // first color is white
    tile_palette_mem->data[2] = rbg(31, 0, 0); // third color is red

    // set the object attributes
    obj_attributes obj = {
	50,  // y coord + other stuff
	50,  // x coord + other stuff
	512 | (0b11 << 0xA) | (0b00 << 0xC), // tile index + others stuff
	0
    };

    // set the lambda attributes
    obj_attributes obj_lambda = {
	100,  // y coord + other stuff
	100,  // x coord + other stuff
	3 | (0b11 << 0xA) | (0b00 << 0xC), // tile index + others stuff
	0
    };
    
    ((volatile obj_attributes*)0x07000000)[0] = obj;
    ((volatile obj_attributes*)0x07000000)[1] = obj_lambda;

    // Wait forever
    while(1)
    {
	vsync_wait();
    }

    return 0;
}
