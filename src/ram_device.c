#include "ram_device.h"

ram_device_t ram_device_init() {
    ram_device_t self;

    for(int i = 0; i < 256*256; i++) {
        self.memory[i] = 0;
    }

    return self;
}

u16 ram_readU16(ram_device_t *self, u16 address) {
    u8 hi = self->memory[address]; 
    u8 lo = self->memory[address + 1]; 
    return (u16) ((hi << 8) | (lo));
}

u8 ram_readU8(ram_device_t *self, u16 address) {
    return self->memory[address];
}

void ram_writeU16(ram_device_t *self, u16 address, u16 data) {
    u8 hi = (u8)(data >> 8);
    u8 lo = (u8)(data & 0xff); 
    self->memory[address] = hi;
    self->memory[address + 1] = lo;
}
void ram_writeU8(ram_device_t *self, u16 address, u8 data) {
    self->memory[address] = data;
}

