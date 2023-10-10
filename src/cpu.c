#include "cpu.h"


cpu_t *cpu_init(u8 *writable_bytes, int nbytes) {
	cpu_t *cpu = (cpu_t *) malloc(sizeof(cpu_t));

	for(int i = 0; i < CPU_REG_COUNT; i++) {
		cpu->registers[i] = 0;
	}

	// Loading instructions into the memory
	for(int i = 0; i < nbytes; i++) {
		cpu->memory[i] = writable_bytes[i];
	}

	return cpu;
}

void cpu_debug(cpu_t *cpu) {
	printf("IP: 0x%04X\n", cpu->registers[CPU_REG_IP]); 
	printf("ACC: 0x%04X\n", cpu->registers[CPU_REG_ACC]); 
	printf("R1: 0x%04X\n", cpu->registers[CPU_REG_R1]); 
	printf("R2: 0x%04X\n", cpu->registers[CPU_REG_R2]); 
	printf("R3: 0x%04X\n", cpu->registers[CPU_REG_R3]); 
	printf("R4: 0x%04X\n", cpu->registers[CPU_REG_R4]); 
	printf("R5: 0x%04X\n", cpu->registers[CPU_REG_R5]); 
	printf("R6: 0x%04X\n", cpu->registers[CPU_REG_R6]); 
	printf("R7: 0x%04X\n", cpu->registers[CPU_REG_R7]); 
	printf("R8: 0x%04X\n", cpu->registers[CPU_REG_R8]); 
	printf("\n");
}

u8 cpu_fetch(cpu_t *cpu) {
	const u16 next_instruction_address = cpu->registers[CPU_REG_IP];
	const u8 instruction = cpu->memory[next_instruction_address];
	cpu->registers[CPU_REG_IP] = next_instruction_address + 1;
	return instruction;
}

u16 cpu_fetch16(cpu_t *cpu) {
	const u16 next_instruction_address = cpu->registers[CPU_REG_IP];
	const u16 ins_hi = cpu->memory[next_instruction_address];
	const u16 ins_lo = cpu->memory[next_instruction_address + 1];
	cpu->registers[CPU_REG_IP] = next_instruction_address + 2;
	return (ins_hi << 8) | (ins_lo);
}

void cpu_execute(cpu_t *cpu, u8 instruction) {
	switch (instruction) {
		case MOV_LIT_R1: {
			const u16 literal = cpu_fetch16(cpu);
			cpu->registers[CPU_REG_R1] = literal;
		} break;
		case MOV_LIT_R2: {
			const u16 literal = cpu_fetch16(cpu);
			cpu->registers[CPU_REG_R2] = literal;
		} break;
		case ADD_REG_REG: {
			const u8 r1 = cpu_fetch(cpu);
			const u8 r2 = cpu_fetch(cpu);
			const u16 regValue1 = cpu->registers[r1];
			const u16 regValue2 = cpu->registers[r2];
			cpu->registers[CPU_REG_ACC] = regValue1 + regValue2;
		} break;
	}
}

void cpu_step(cpu_t *cpu) {
	u8 instruction = cpu_fetch(cpu);
	cpu_execute(cpu, instruction);
}
