#include <stdio.h>

#include "cpu.h"
#include "instructions.h"

int main(int argc, char **argv) {
	int i = 0;
	u8 writable_bytes[9];

	writable_bytes[i++] = MOV_LIT_R1;
	writable_bytes[i++] = 0x12;
	writable_bytes[i++] = 0x34;

	writable_bytes[i++] = MOV_LIT_R2;
	writable_bytes[i++] = 0xAB;
	writable_bytes[i++] = 0xCD;

	writable_bytes[i++] = ADD_REG_REG;
	writable_bytes[i++] = CPU_REG_R1;
	writable_bytes[i++] = CPU_REG_R2;

	cpu_t *cpu = cpu_init(writable_bytes, 9);

	cpu_debug(cpu);

	cpu_step(cpu);
	cpu_debug(cpu);

	cpu_step(cpu);
	cpu_debug(cpu);

	cpu_step(cpu);
	cpu_debug(cpu);

	return 0;
}
