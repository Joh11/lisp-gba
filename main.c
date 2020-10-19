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
    REG_DISPCNT = flag_video_mode0;
    init_keyboard();
    
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
