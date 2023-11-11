#ifndef RAM_DEVICE_H
#define RAM_DEVICE_H

#include "base_types.h"
#include <stdio.h>

typedef struct {
    u8 memory[(256*256) + 1];
} ram_device_t;

ram_device_t ram_device_init();

u16 ram_readU16(ram_device_t *self, u16 address);
u8 ram_readU8(ram_device_t *self, u16 address);

void ram_writeU16(ram_device_t *self, u16 address, u16 data);
void ram_writeU8(ram_device_t *self, u16 address, u8 data);


#endif
