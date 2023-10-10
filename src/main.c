#include <stdio.h>

#include "cpu.h"
#include "instructions.h"
#include "base_helper.h"

int main(int argc, char **argv) {
	u8 writable_bytes[] = {
		// start:
		//	mov mem[0x0100], r1
		MOV_MEM_REG,
		0x01,
		0x00,
		CPU_REG_R1,
		//	mov 0x0001, r2
		MOV_LIT_REG,
		0x00,
		0x01,
		CPU_REG_R2,
		//	add r1, r2
		ADD_REG_REG,
		CPU_REG_R1,
		CPU_REG_R2,
		//	mov acc, mem[0x0100]
		MOV_REG_MEM,
		CPU_REG_ACC,
		0x01,
		0x00,
		//	jne 0x0003, start
		JNE_LIT,
		0x00,
		0x03,
		0x00,
		0x00,
		
	};

	cpu_t *cpu = cpu_init(writable_bytes, ArrayCount(writable_bytes));

	cpu_debug(cpu);
	cpu_viewMemoryAt(cpu, cpu->registers[CPU_REG_IP]);
	cpu_viewMemoryAt(cpu, 0x0100);

	char c;
	while (1) {
		c = getchar();
		if (c == '\n') {
			cpu_step(cpu);
			cpu_debug(cpu);
			cpu_viewMemoryAt(cpu, cpu->registers[CPU_REG_IP]);
			cpu_viewMemoryAt(cpu, 0x0100);
		}
		else if(c == 'q') break;

		while (getchar() != '\n');
	}


	return 0;
}
