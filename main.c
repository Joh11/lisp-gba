#include "constants.h"

#define RED   0x001F
#define GREEN 0x03E0
#define BLUE  0x7C00

#define NULL 0

static inline void vsync_wait()
{
    while(REG_VCOUNT >= 160); // wait till VDraw
    while(REG_VCOUNT < 160);  // wait till VBlank
}

static inline uint16 rbg(int r, int g, int b)
{
    return (b << 10) | (g << 5) | r;
}

int main(void)
{
    *REG_DISPCNT = flag_video_mode0 | flag_enable_sprites;

    // sprite 0 of reg 5 will be totally white (the 2nd color of the palette)
    tile4 white_sprite = {
	{0x11111111, 0x22222222, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111},
    };
    tile_mem[5][0] = white_sprite;
    tile_mem[4][1] = white_sprite;
    tile_mem[4][2] = white_sprite;

    // set the palette
    tile_palette_mem->data[0] = rbg(0, 31, 0); // transparency color is black
    tile_palette_mem->data[1] = rbg(31, 31, 31); // first color is white
    tile_palette_mem->data[2] = rbg(31, 0, 0); // third color is red

    // set the object attributes
    obj_attributes obj = {
	50,  // y coord + other stuff
	50,  // x coord + other stuff
	512 | (0b11 << 0xA) | (0b00 << 0xC), // tile index + others stuff
	NULL
    };

    ((volatile obj_attributes*)0x07000000)[0] = obj;

    uint32 x = sizeof(tile4);
    
    // Wait forever
    while(1)
    {
	vsync_wait();
    }

    return 0;
}
