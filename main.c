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
	    keyboard_handle_keypress(ENUM_UP);
	if(key_pressed_delay(ENUM_DOWN))
	    keyboard_handle_keypress(ENUM_DOWN);
	if(key_pressed_delay(ENUM_LEFT))
	    keyboard_handle_keypress(ENUM_LEFT);
	if(key_pressed_delay(ENUM_RIGHT))
	    keyboard_handle_keypress(ENUM_RIGHT);

	if(key_pressed_delay(ENUM_START))
	    keyboard_handle_keypress(ENUM_START);
	if(key_pressed_delay(ENUM_A))
	    keyboard_handle_keypress(ENUM_A);
	if(key_pressed_delay(ENUM_B))
	    keyboard_handle_keypress(ENUM_B);
	
	update_selector();
	update_key_state();
    }

    return 0;
}
