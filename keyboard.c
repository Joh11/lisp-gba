#include <stddef.h>

#include "image.h"
#include "keyboard.h"

// -----------------------------------------------------------------------------
// Constants
// -----------------------------------------------------------------------------

static const uint32 big_selector[] = {0x10177, 0x10101, 0x8080ee, 0x808080, 0x1010100, 0x77010100, 0x808080, 0xee808000};

// tiles for the keycaps
static const bitpacked_tile4 bg_tile = {0x00000000, 0x00000000};
static const uint32 keycap[] = {0xfdfd01ff, 0xfdfdfdfd, 0xbfbf80ff, 0xbfbfbfbf, 0xfdfdfdfd, 0xff01fdfd, 0xbfbfbfbf, 0xff80bfbf};

// Font
static const uint32 font[] = {0xf7f3f7ff, 0xe3f7f7f7, 0xdfdbe7ff, 0xc3fbf7ef, 0xdfdbe7ff, 0xe7dbdfe7, 0xdbd7cfff, 0xdfdfdfc3, 0xe3fbc3ff, 0xe3dfdfdf, 0xfbf7efff, 0xe7dbdbe3, 0xdfdfc3ff, 0xf7f7f7ef, 0xdbdbe7ff, 0xe7dbdbe7, 0xdbdbe7ff, 0xf7efdfc7, 0xcbdbe7ff, 0xe7dbdbd3, 0xffffffff, 0xffffffff, 0xdbdbe7ff, 0xdfdfc7db, 0xffffffff, 0xebd5d5dd, 0xe7ffffff, 0xc7fbc3db, 0xcbffffff, 0xfbfbfbf3, 0xfbe1fbff, 0xf7ebfbfb, 0xdbdbdbff, 0xe7dfdfc7, 0xdbffffff, 0xc7dbdbdb, 0xefffffff, 0xefefefff, 0xe7ffffff, 0xe7dbdbdb, 0xdbdbe7ff, 0xfbfbfbe3, 0xffffffff, 0xffffffff, 0xe7ffffff, 0xc7dbc7df, 0xc7ffffff, 0xe3dfe7fb, 0xdfdfffff, 0xc7dbdbc7, 0xfbdbe7ff, 0xfbfbe3fb, 0xdbc7ffff, 0xe7dfc7db, 0xfbfbffff, 0xdbdbdbe3, 0xefffefff, 0xf7ebefef, 0xdbdbfbff, 0xdbdbdbe3, 0xfbfbfbff, 0xf7fbfbfb, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xc3ffffff, 0xc3fbe7df, 0xdbffffff, 0xdbdbe7db, 0xe7ffffff, 0xe7dbfbdb, 0xbbffffff, 0xefd7bbbb, 0xfbfbfbff, 0xe3dbdbe3, 0xffffffff, 0xdbdbdbe3, 0xffffffff, 0xabababc3, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};


#define LINE_LEN 30
static const uint8 char_lut[44] = "1234567890'qwertyuiop.asdfghjkl;,zxcvbnm(-+=";

// -----------------------------------------------------------------------------
// Local variables
// -----------------------------------------------------------------------------

static obj_attributes obj_selector;
static bool keyboard_visiblep = true;

typedef struct
{
    uint16 x;
    uint16 y;
} PACKED key_pos;

static key_pos selector_pos = {0, 0};

static uint8 text[LINE_LEN];
static uint32 text_pos = 0; // textpos = 0 for an empty text

// -----------------------------------------------------------------------------
// Functions
// -----------------------------------------------------------------------------

void init_keyboard()
{
    // -------------------------------------------------------------------------
    // Registers
    // -------------------------------------------------------------------------
    
    REG_DISPCNT |= flag_enable_bg0 | flag_enable_sprites | flag_1d_mapping;
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

    for(size_t i = 0 ; i < LINE_LEN ; ++i)
	text[i] = 255;
    
    // -------------------------------------------------------------------------
    // Init sprites
    // -------------------------------------------------------------------------

    // palette
    tile_palette_mem->data[0] = rbg(31, 0, 31); // transparency color is magenta
    tile_palette_mem->data[1] = rbg(31, 31, 31); // first color is white
    tile_palette_mem->data[2] = rbg(31, 0, 0); // third color is red

    // tiles
    unpack_monochrome_tiles(big_selector, &tile_mem[4][1], 4, 0x2, 0x0);
    unpack_monochrome_tiles(font, &tile_mem[4][5], 44, 0x0, 0x1);

    // sprite
    obj_selector.attr0 = 96 // y coord
	| (0b00 << 0xe); // sprite shape (16x16)
    obj_selector.attr1 = 32 // x coord
	| (0b01 << 0xe); // sprite size (16x16)
    obj_selector.attr2 = 1 // tile index
	| (0b10 << 0xA) // priority
	| (0b00 << 0xC); // palette-bank

    oam[0] = obj_selector;
    
    // font object attributes
    for(size_t i = 0; i < 44 ; ++i)
    {
	oam[1+i].attr0 = 100 + 16 * (i / 11) // y coord
	    | (0b00 << 0xe); // sprite shape (8x8)
	oam[1+i].attr1 = 36 + 16 * (i % 11) // x coord
	    | (0b00 << 0xe); // sprite size (8x8)
	oam[1+i].attr2 = 5+i // tile index
	    | (0b10 << 0xA) // priority
	    | (0b00 << 0xC); // palette-bank
    }

    // Line
    update_line();
    
    // -------------------------------------------------------------------------
    // Init backgrounds
    // -------------------------------------------------------------------------

    // palette
    bg_palette_mem->data[0] = rbg(0, 0, 0); // transparency color is black
    bg_palette_mem->data[1] = rbg(31, 31, 31); // first color is white
    bg_palette_mem->data[2] = rbg(15, 15, 15); // second color is gray

    // tiles (use charblock 1)
    tile_mem[1][0] = unpack_monochrome_tile4(bg_tile, 1, 0);
    tile_mem[1][1] = unpack_monochrome_tile4(bg_tile, 1, 2);
    unpack_monochrome_tiles(keycap, &tile_mem[1][2], 4, 0x0, 0x2);

    // tile map
    fill_keyboard_tilemap(se_mem[0]);
}

void fill_keyboard_tilemap(volatile screenblock sb)
{
    for(size_t row = 0 ; row < 12 ; ++row)
    {
	for(size_t col = 0 ; col < 32 ; ++col)
	    sb[32 * row + col] = 0;
    }
    for(size_t row = 12 ; row < 20 ; ++row)
    {
	for(size_t col = 0 ; col < 32 ; ++col)
	{
	    if ((col < 4) || (col > 25))
	    {
		sb[32 * row + col] = 0;
		continue;
	    }
	    sb[32 * row + col] = (row % 2) ?
		4 + (col % 2)
		: 2 + (col % 2);
	}
    }
    for(size_t row = 20 ; row < 32 ; ++row)
    {
	for(size_t col = 0 ; col < 32 ; ++col)
	    sb[32 * row + col] = 0;
    }
}

void update_selector()
{
    oam[0] = obj_selector;
}

void update_line()
{
    for(size_t i = 0; i < LINE_LEN ; ++i)
    {
	oam[45+i].attr0 = 0 // y coord
	    | (0b00 << 0xe); // sprite shape (8x8)
	oam[45+i].attr1 = 8 * i // x coord
	    | (0b00 << 0xe); // sprite size (8x8)
	oam[45+i].attr2 = ((text[i] == 255) ? 0 : 5+text[i]) // tile index
	    | (0b10 << 0xA) // priority
	    | (0b00 << 0xC); // palette-bank
    }
}

void keyboard_handle_keypress(enum_key key)
{
    if(!keyboard_visiblep && (key != ENUM_START))
	return;
    
    switch(key)
    {
    case ENUM_A:
	keyboard_type();
	break;
    case ENUM_B:
	keyboard_erase();
	break;
    case ENUM_SELECT:
	// modifier key
	break;
    case ENUM_START:
	toggle_keyboard_visibility();
	break;
    case ENUM_LEFT:
    case ENUM_RIGHT:
    case ENUM_UP:
    case ENUM_DOWN:
	keyboard_move(key);
	break;
    case ENUM_R:
	// 
	break;
    case ENUM_L:
	// 
	break;
    default:
	break;
    }
}

void toggle_keyboard_visibility()
{
    keyboard_visiblep = !keyboard_visiblep;
    obj_selector.attr0 ^= (0b1 << 9); // selector
    for(size_t i = 1 ; i < 45 ; ++i) // keys
	oam[i].attr0 ^= (0b1 << 9);
    REG_DISPCNT ^= flag_enable_bg0;   // background
}

void keyboard_move(enum_key key)
{
    switch(key)
    {
    case ENUM_UP:
	if(selector_pos.y == 0)
	    return;
	selector_pos.y--;
	break;
    case ENUM_DOWN:
	if(selector_pos.y == 3)
	    return;
	selector_pos.y++;
	break;
    case ENUM_LEFT:
	if(selector_pos.x == 0)
	    return;
	selector_pos.x--;
	break;
    case ENUM_RIGHT:
	if(selector_pos.x == 10)
	    return;
	selector_pos.x++;
	break;
    default:
	return;
	break;
    }
    
    // if arrived here, update obj_selector screen pos
    obj_selector.attr0 = (96 + 16 * selector_pos.y)
	| (obj_selector.attr0 & 0xff00);
    obj_selector.attr1 = (32 + 16 * selector_pos.x)
	| (obj_selector.attr1 & 0xff00);

}

void keyboard_type()
{
    if(text_pos >= LINE_LEN)
	return;

    text[text_pos] = 11 * selector_pos.y + selector_pos.x;
    text_pos++;
    update_line();
}

void keyboard_erase()
{
    if(text_pos == 0)
	return;

    text_pos--;
    text[text_pos] = 255;
    update_line();
}
