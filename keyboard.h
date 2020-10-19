#pragma once

#include "constants.h"

void init_keyboard();

void fill_keyboard_tilemap(volatile screenblock sb);

void update_selector();
void move_cursor(uint32 dx, uint32 dy);
