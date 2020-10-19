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
    
    // Wait forever
    while(1)
    {
	vsync_wait();

	if(key_pressed_delay(ENUM_UP))
	    move_cursor(0, -16);
	if(key_pressed_delay(ENUM_DOWN))
	    move_cursor(0, 16);
	if(key_pressed_delay(ENUM_LEFT))
	    move_cursor(-16, 0);
	if(key_pressed_delay(ENUM_RIGHT))
	    move_cursor(16, 0);
	
	update_selector();
	update_key_state();

    }

    return 0;
}
