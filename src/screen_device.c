#include "screen_device.h"

#include <math.h>
#include <stdio.h>

screen_device_t screen_device_init() {
    screen_device_t screen;

    return screen;
}

u16 screen_device_readU16(screen_device_t *self, u16 address) {
    return 0;
}
u8 screen_device_readU8(screen_device_t *self, u16 address) {
    return 0;
}

void screen_device_writeU16(screen_device_t *self, u16 address, u16 data) {

}

void screen_device_writeU8(screen_device_t *self, u16 address, u8 data) {

}
