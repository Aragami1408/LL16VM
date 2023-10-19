#include <stdio.h>
#include <string.h>

#include "cpu.h"
#include "instructions.h"
#include "memory_mapper.h"
#include "ram_device.h"
#include "screen_device.h"
#include "base_helper.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <SDL.h>

int i = 0;

void write_char_to_screen(u8 *writable_bytes, char c, int position) {
	writable_bytes[i++] = MOV_LIT_REG;
	writable_bytes[i++] = 0x00;
	writable_bytes[i++] = (u8) c;
	writable_bytes[i++] = CPU_REG_R1;

	writable_bytes[i++] = MOV_REG_MEM;
	writable_bytes[i++] = CPU_REG_R1;
	writable_bytes[i++] = 0x30;
	writable_bytes[i++] = position;
}

int main(int argc, char **argv) {
	memmapper_t *mm = memmapper_create();

	ram_device_t ram = ram_device_init();	
	screen_device_t screen = screen_device_init();
	memmapper_map(mm, DEVICE_RAM, &ram, 0x0000, 0xffff, false);
	memmapper_map(mm, DEVICE_SCREEN, &screen, 0x3000, 0x30ff, true);

	u8 *writable_bytes = ram.memory;

	cpu_t *cpu = cpu_init(mm);

	const char *str = "hello world!";

	for(int index = 0; index <= 0xff; index++) {
		write_char_to_screen(writable_bytes, '*', index);
	}

	writable_bytes[i++] = HLT;

	cpu_run(cpu);

	memmapper_free(mm);
	cpu_free(cpu);

	return 0;
}
