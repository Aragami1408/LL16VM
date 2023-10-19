#ifndef MEMORY_MAPPER_H
#define MEMORY_MAPPER_H

#include "base_types.h"
#include "ram_device.h"
#include "screen_device.h"

#include <stdbool.h>
#include <stdlib.h>

typedef enum {
    DEVICE_RAM,
    DEVICE_SCREEN,
    DEVICE_COUNT
} device_type_t;


typedef struct memmapper_region {
    device_type_t type;
    void *device;
    u16 start;
    u16 end;
    bool remap;
} memmapper_region_t;

typedef struct memmapper {
    memmapper_region_t regions[DEVICE_COUNT];
} memmapper_t;

memmapper_t *memmapper_create();
void memmapper_free(memmapper_t *self);

void memmapper_map(
    memmapper_t *self, 
    device_type_t type,
    void *device, 
    u16 start, u16 end, 
    bool remap);

memmapper_region_t *memmapper_findRegion(memmapper_t *self, u16 address);

u16 memmapper_readU16(memmapper_t *self, u16 address);
u8 memmapper_readU8(memmapper_t *self, u16 address);

void memmapper_writeU16(memmapper_t *self, u16 address, u16 data);
void memmapper_writeU8(memmapper_t *self, u16 address, u8 data);

#endif
