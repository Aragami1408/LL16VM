#include "screen_device.h"

#include <math.h>
#include <stdio.h>

void moveTo(int x, int y) {
    printf("\x1b[%d;%dH", y, x);
}

u16 screen_device_readU16(screen_device_t *self, u16 address) {
    return 0;
}
u8 screen_device_readU8(screen_device_t *self, u16 address) {
    return 0;
}

screen_device_t screen_device_init() {
    screen_device_t screen;

    return screen;
}

void screen_device_writeU16(screen_device_t *self, u16 address, u16 data) {
    const u8 character_value = data & 0x00ff;

    const int x = address % 16;
    const int y = (int) floor(address / 16); 
    moveTo(x*2, y);
    printf("%c\n", (char) character_value);
    // printf("x=%d, y=%d, char=%c\n", x, y, character_value);
}
void screen_device_writeU8(screen_device_t *self, u16 address, u8 data) {

}