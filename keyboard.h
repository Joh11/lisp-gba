#pragma once

#include "constants.h"
#include "input.h"

void init_keyboard();

void fill_keyboard_tilemap(volatile screenblock sb);

void update_selector();

void keyboard_handle_keypress(enum_key key);
void toggle_keyboard_visibility();
void keyboard_move(enum_key key);

// Add the selected char to the text
void keyboard_type();
void keyboard_erase();
