#pragma once

#include "constants.h"

tile4 unpack_monochrome_tile4(const uint32 data[2], uint8 fg_index, uint8 gb_index);

void unpack_monochrome_tiles(const uint32* data, tile4* dest, uint32 num_tiles, uint8 fg_index, uint8 gb_index);
