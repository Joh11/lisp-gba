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

// Vsync
#define REG_VCOUNT *(volatile uint16*)0x04000006

// General settings for display
#define REG_DISPCNT (volatile uint16*)0x04000000

// Flags
#define flag_video_mode0 0
#define flag_enable_sprites (1 << 12)


// Object attributes stuff
typedef struct {
    uint16 attr0;
    uint16 attr1;
    uint16 attr2;
    sint16 fill;
} ALIGN(8) obj_attributes;
