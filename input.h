#pragma once

#include "constants.h"

#define REG_KEYINPUT *(volatile uint32*)0x04000130

#define KEY_A      (0b1 << 0)
#define KEY_B      (0b1 << 1)
#define KEY_SELECT (0b1 << 2)
#define KEY_START  (0b1 << 3)
#define KEY_RIGHT  (0b1 << 4)
#define KEY_LEFT   (0b1 << 5)
#define KEY_UP     (0b1 << 6)
#define KEY_DOWN   (0b1 << 7)
#define KEY_R      (0b1 << 8)
#define KEY_L      (0b1 << 9)
#define KEY_ANY    0x03ff

// Same but the enum is not bitshifted
typedef enum
{
    ENUM_A      = 0,
    ENUM_B      = 1,
    ENUM_SELECT = 2,
    ENUM_START  = 3,
    ENUM_RIGHT  = 4,
    ENUM_LEFT   = 5,
    ENUM_UP     = 6,
    ENUM_DOWN   = 7,
    ENUM_R      = 8,
    ENUM_L      = 9
} enum_key;

bool is_key_pressed(enum_key key);

// Same as is_key_pressed, but if a button is pressed, wait `delay`
// frames before returning true again.
bool key_pressed_delay(enum_key key);

// Set the delay between two consecutive keypresses if the key stays
// pressed
void set_key_delay(uint32 delay);

// Call this every frame to make sure the delay feature works
void update_key_state();
