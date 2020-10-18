#include <string.h>

#include "input.h"

static uint32 input_delay = 5;
static uint32 input_cycles[10] = {0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0};
static bool input_onpressed_frame[10] = {0, 0, 0, 0, 0,
					 0, 0, 0, 0, 0};

bool is_key_pressed(enum_key key)
{
    return (0b1 << key) & (~REG_KEYINPUT & KEY_ANY);
}

bool key_pressed_delay(enum_key key)
{
    return input_onpressed_frame[key];
}

void set_key_delay(uint32 delay)
{
    input_delay = delay;
}

void update_key_state()
{
    // Reset the onpressed booleans
    memset(input_onpressed_frame, 0, 10 * sizeof(bool));
    
    for(enum_key key = 0; key < 10; ++key)
    {
	if(is_key_pressed(key))
	{
	    if(input_cycles[key] >= input_delay)
	    {
		input_cycles[key] = 0;
		input_onpressed_frame[key] = true;
	    }
	    else
		input_cycles[key]++;
	
	}
	else if(input_cycles[key] != 0)
	    input_cycles[key] = input_delay;
    }
}
