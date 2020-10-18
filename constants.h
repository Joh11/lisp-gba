#pragma once

#define ALIGN(n) __attribute__((aligned(n)))

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef signed char sint8;
typedef signed short sint16;
typedef signed int sint32;

typedef uint32 bool; // As it is a 32bit system it should be faster TODO check that
#define true 1
#define false 1

typedef struct { uint32 data[8]; } tile4;
typedef struct { uint32 data[16]; } tile8;

typedef tile4 charblock[512];
typedef tile8 charblock8[256];

#define tile_mem  ((charblock*) 0x06000000)
#define tile8_mem ((charblock8*)0x06000000)

typedef struct { uint16 data[256]; } palette;

#define bg_palette_mem   ((volatile palette*) 0x05000000)
#define tile_palette_mem ((volatile palette*) 0x05000200)

#define oam ((volatile obj_attributes*)0x07000000) // object attribute memory

// Vsync
#define REG_VCOUNT *(volatile uint16*)0x04000006

// General settings for display
#define REG_DISPCNT *(volatile uint16*)0x04000000

// Flags
#define flag_video_mode0 0
#define flag_enable_bg0 (1 << 8)
#define flag_enable_bg1 (1 << 9)
#define flag_enable_bg2 (1 << 0xa)
#define flag_enable_bg3 (1 << 0xb)
#define flag_enable_sprites (1 << 12)


// Object attributes stuff
typedef struct {
    uint16 attr0;
    uint16 attr1;
    uint16 attr2;
    sint16 fill;
} ALIGN(8) obj_attributes;


// Background stuff

#define REG_BG0CNT *(volatile uint16*)0x04000008
#define REG_BG1CNT *(volatile uint16*)0x0400000a
#define REG_BG2CNT *(volatile uint16*)0x0400000c
#define REG_BG3CNT *(volatile uint16*)0x0400000e

#define REG_BG0HOFS *(volatile uint16*)0x04000010
#define REG_BG0VOFS *(volatile uint16*)0x04000012
#define REG_BG1HOFS *(volatile uint16*)0x04000014
#define REG_BG1VOFS *(volatile uint16*)0x04000016
#define REG_BG2HOFS *(volatile uint16*)0x04000018
#define REG_BG2VOFS *(volatile uint16*)0x0400001a
#define REG_BG3HOFS *(volatile uint16*)0x0400001c
#define REG_BG3VOFS *(volatile uint16*)0x0400001e

typedef uint16 screen_entry;
typedef screen_entry screenblock[1024];

// Screen entry mapping: se_mem[x][y] is screen base block x, entry y
#define se_mem ((volatile screenblock*)0x06000000)

typedef uint32 bitpacked_tile4[2];
