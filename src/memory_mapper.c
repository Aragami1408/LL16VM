#include "memory_mapper.h"

memmapper_t *memmapper_create() {
    memmapper_t *self = (memmapper_t*)malloc(sizeof(memmapper_t));

    return self;
}

void memmapper_free(memmapper_t *self) {
    free(self);
}

void memmapper_map(
    memmapper_t *self, 
    device_type_t type,
    void *device, 
    u16 start, u16 end, 
    bool remap) {

    memmapper_region_t region;
    region.type = type;
    region.device = device;
    region.start = start;
    region.end = end;
    region.remap = remap;
    self->regions[self->region_count] = region;
    self->region_count++;
}

memmapper_region_t *memmapper_findRegion(memmapper_t *self, u16 address) {
    for (int i = 0; i < self->region_count; i++) {
        memmapper_region_t *region = &self->regions[i];
        if (address >= region->start && address <= region->end)
        {
            return region;
        }
    }
    
    printf("No memory region found for address %d\n", address);
    exit(EXIT_FAILURE);
}

u16 memmapper_readU16(memmapper_t *self, u16 address) {
    memmapper_region_t *region = memmapper_findRegion(self, address);
    if (region) {
        u16 final_addr = region->remap ? address - region->start : address;

        switch (region->type) {
        case DEVICE_RAM: 
            return ram_readU16((ram_device_t *)region->device, final_addr);
        case DEVICE_SCREEN:
            return screen_device_readU16((screen_device_t *)region->device, final_addr);
        default:
            break;
        }

        return 0; // Replace this with the actual logic.
    }
    else {
        // TODO: Error handling
        return 0;
    }
}

u8 memmapper_readU8(memmapper_t *self, u16 address) {
    memmapper_region_t *region = memmapper_findRegion(self, address);
    if (region) {
        u16 final_addr = region->remap ? address - region->start : address;

        switch (region->type) {
        case DEVICE_RAM: 
            return ram_readU8((ram_device_t *)region->device, final_addr);
        case DEVICE_SCREEN:
            return screen_device_readU8((screen_device_t *)region->device, final_addr);
        default:
            break;
        }

        // TODO: Implement the logic to read the u16 value from the device here
        // Replace the following line with the actual implementation
        return 0; // Replace this with the actual logic.
    }
    else {
        // TODO: Error handling
        return 0;
    }
}

void memmapper_writeU16(memmapper_t *self, u16 address, u16 data){
    memmapper_region_t *region = memmapper_findRegion(self, address);
    if (region) {
        u16 final_addr = region->remap ? address - region->start : address;

        switch (region->type)
        {
        case DEVICE_RAM: 
            ram_writeU16((ram_device_t *)region->device, final_addr, data);
        case DEVICE_SCREEN:
            screen_device_writeU16((screen_device_t *)region->device, final_addr, data);
        default:
            break;
        }
        // TODO: Implement the logic to write the u8 value from the device here
        // Replace the following line with the actual implementation
    }
    else {
        // TODO: Error handling
    }
}

void memmapper_writeU8(memmapper_t *self, u16 address, u8 data){
    memmapper_region_t *region = memmapper_findRegion(self, address);
    if (region) {
        u16 final_addr = region->remap ? address - region->start : address;

        switch (region->type)
        {
        case DEVICE_RAM: 
            ram_writeU8((ram_device_t*)region->device, final_addr, data);
        case DEVICE_SCREEN:
            screen_device_writeU8((screen_device_t *)region->device, final_addr, data);
        default:
            break;
        }
        // TODO: Implement the logic to write the u8 value from the device here
        // Replace the following line with the actual implementation
    }
    else {
        // TODO: Error handling
    }

}