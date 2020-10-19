#include <stdlib.h>

#include "constants.h"
#include "input.h"
#include "keyboard.h"
#include "image.h"

static inline void vsync_wait()
{
    while(REG_VCOUNT >= 160); // wait till VDraw
    while(REG_VCOUNT < 160);  // wait till VBlank
}

int main(void)
{
    REG_DISPCNT = flag_video_mode0 | flag_enable_bg0 | flag_enable_sprites | flag_1d_mapping;

    init_keyboard();
    
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

	/* if(key_pressed_delay(ENUM_UP)) */
	/*     obj_selector.attr0 -= 16; */
	/* if(key_pressed_delay(ENUM_DOWN)) */
	/*     obj_selector.attr0 += 16; */
	/* if(key_pressed_delay(ENUM_LEFT)) */
	/*     obj_selector.attr1 -= 16; */
	/* if(key_pressed_delay(ENUM_RIGHT)) */
	/*     obj_selector.attr1 += 16; */
	
	/* oam[0] = obj_selector; */
	
	update_key_state();

    }

    return 0;
}
