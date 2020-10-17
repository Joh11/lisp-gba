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

bool is_key_pressed(uint32 key);
