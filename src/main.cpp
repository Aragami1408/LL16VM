#include <stdio.h>

#include "cpu.h"
#include "instructions.h"
#include "memory_mapper.h"
#include "ram_device.h"
#include "base_helper.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <SDL.h>

int main(int argc, char **argv) {
	memmapper_t *mm = memmapper_create();

	ram_device_t ram = ram_device_init();	
	memmapper_map(mm, DEVICE_RAM, &ram, 0x0000, 0xffff, false);

	u8 *writable_bytes = ram.memory;

	cpu_t *cpu = cpu_init(mm);

	int i = 0;

	writable_bytes[i++] = MOV_LIT_REG;
	writable_bytes[i++] = 0x00;
	writable_bytes[i++] = 'H';
	writable_bytes[i++] = CPU_REG_R1;

	writable_bytes[i++] = MOV_REG_MEM;
	writable_bytes[i++] = CPU_REG_R1;
	writable_bytes[i++] = 0x30;
	writable_bytes[i++] = 0x00;

	writable_bytes[i++] = HLT;

	cpu_run(cpu);

	return 0;
}
