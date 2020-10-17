#include "input.h"

bool is_key_pressed(uint32 key)
{
    return key & (~REG_KEYINPUT & KEY_ANY);
}
