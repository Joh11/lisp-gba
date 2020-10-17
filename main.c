#include "constants.h"

#define RED   0x001F
#define GREEN 0x03E0
#define BLUE  0x7C00

static inline void vsync_wait()
{
    while(REG_VCOUNT >= 160); // wait till VDraw
    while(REG_VCOUNT < 160);  // wait till VBlank
}

int main(void)
{
    // Write into the I/O registers, setting video display parameters.
    volatile uint8 *ioram = (uint8 *)0x04000000;
    ioram[0] = 0x03; // Use video mode 3 (in BG2, a 16bpp bitmap in VRAM)
    ioram[1] = 0x04; // Enable BG2 (BG0 = 1, BG1 = 2, BG2 = 4, ...)

    // Write pixel colours into VRAM
    volatile uint16 *vram = (uint16 *)0x06000000;
    vram[40*240 + 115] = RED; // X = 115, Y = 80, C = 000000000011111 = R
    vram[80*240 + 120] = GREEN; // X = 120, Y = 80, C = 000001111100000 = G
    vram[80*240 + 125] = BLUE; // X = 125, Y = 80, C = 111110000000000 = B

    // Wait forever
    while(1)
    {
	vsync_wait();
    }

    return 0;
}
