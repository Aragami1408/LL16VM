#ifndef SCREEN_DEVICE_H
#define SCREEN_DEVICE_H

#include "base_types.h"

typedef struct {
    // it's empty for now
} screen_device_t;

screen_device_t screen_device_init();

u16 screen_device_readU16(screen_device_t *self, u16 address);
u8 screen_device_readU8(screen_device_t *self, u16 address);

void screen_device_writeU16(screen_device_t *self, u16 address, u16 data);
void screen_device_writeU8(screen_device_t *self, u16 address, u8 data);

#endif