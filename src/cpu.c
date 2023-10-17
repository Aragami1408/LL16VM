#include "cpu.h"
#include "base_helper.h"
#include "instructions.h"

cpu_t *cpu_init(u8 *writable_bytes, int nbytes) {
	cpu_t *cpu = (cpu_t *) malloc(sizeof(cpu_t));

	for(int i = 0; i < CPU_REG_COUNT; i++) {
		cpu->registers[i] = 0;
	}

	// Loading instructions into the memory
	for(int i = 0; i < nbytes; i++) {
		cpu->memory[i] = writable_bytes[i];
	}

	cpu->registers[CPU_REG_SP] = ArrayCount(cpu->memory) - 1 - 1;
	cpu->registers[CPU_REG_FP] = ArrayCount(cpu->memory) - 1 - 1;

	cpu->stackframe_size = 0;

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
	printf("SP: 0x%04X\n", cpu->registers[CPU_REG_SP]); 
	printf("FP: 0x%04X\n", cpu->registers[CPU_REG_FP]); 
	printf("\n");
}

void cpu_viewMemoryAt(cpu_t *cpu, u16 address) {
	printf("0x%04X: ", address);
	for(int i = 0; i < 8; i++) 
		printf("0x%02X ", cpu->memory[address+i]);	

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
	return (u16) ((ins_hi << 8) | (ins_lo));
}

u16 cpu_memRead16(cpu_t *cpu, u16 address) {
	u8 hi = cpu->memory[address];
	u8 lo = cpu->memory[address + 1];

	return (u16) ((hi << 8) | (lo));
}

void cpu_memWrite16(cpu_t *cpu, u16 address, u16 data) {
	u8 hi = (u8) (data >> 8);	
	u8 lo = (u8) (data & 0xff);
	cpu->memory[address] = hi;
	cpu->memory[address + 1] = lo;
}

void cpu_stackPush(cpu_t *cpu, u16 value) {
	const u16 sp_address = cpu->registers[CPU_REG_SP];
	cpu_memWrite16(cpu, sp_address, value);
	cpu->registers[CPU_REG_SP] = sp_address - 2;
	cpu->stackframe_size += 2;
}

void cpu_stackPushState(cpu_t *cpu) {
	cpu_stackPush(cpu, cpu->registers[CPU_REG_R1]);
	cpu_stackPush(cpu, cpu->registers[CPU_REG_R2]);
	cpu_stackPush(cpu, cpu->registers[CPU_REG_R3]);
	cpu_stackPush(cpu, cpu->registers[CPU_REG_R4]);
	cpu_stackPush(cpu, cpu->registers[CPU_REG_R5]);
	cpu_stackPush(cpu, cpu->registers[CPU_REG_R6]);
	cpu_stackPush(cpu, cpu->registers[CPU_REG_R7]);
	cpu_stackPush(cpu, cpu->registers[CPU_REG_R8]);
	cpu_stackPush(cpu, cpu->registers[CPU_REG_IP]);
	cpu_stackPush(cpu, (u16) cpu->stackframe_size + 2);

	cpu->registers[CPU_REG_FP] = cpu->registers[CPU_REG_SP];
	cpu->stackframe_size = 0;
}


u16 cpu_stackPop(cpu_t *cpu) {
	const u16 next_sp_address = cpu->registers[CPU_REG_SP] + 2;
	cpu->registers[CPU_REG_SP] = next_sp_address;
	cpu->stackframe_size -= 2;
	return cpu_memRead16(cpu, next_sp_address);
}

void cpu_stackPopState(cpu_t *cpu) {
	const u16 fp_address = cpu->registers[CPU_REG_FP];
	cpu->registers[CPU_REG_FP] = fp_address;

	cpu->stackframe_size = cpu_stackPop(cpu);
	const u16 stackframe_size = cpu->stackframe_size;

	cpu->registers[CPU_REG_IP] = cpu_stackPop(cpu);
	cpu->registers[CPU_REG_R8] = cpu_stackPop(cpu);
	cpu->registers[CPU_REG_R7] = cpu_stackPop(cpu);
	cpu->registers[CPU_REG_R6] = cpu_stackPop(cpu);
	cpu->registers[CPU_REG_R5] = cpu_stackPop(cpu);
	cpu->registers[CPU_REG_R4] = cpu_stackPop(cpu);
	cpu->registers[CPU_REG_R3] = cpu_stackPop(cpu);
	cpu->registers[CPU_REG_R2] = cpu_stackPop(cpu);
	cpu->registers[CPU_REG_R1] = cpu_stackPop(cpu);

	const u16 nargs = cpu_stackPop(cpu);
	for(int i = 0; i < nargs; i++) {
		cpu_stackPop(cpu);
	}

	cpu->registers[CPU_REG_FP] = fp_address + stackframe_size;
}

void cpu_execute(cpu_t *cpu, u8 instruction) {
	switch (instruction) {
		case MOV_LIT_REG: {
			const u16 literal = cpu_fetch16(cpu);
			const u8 reg = cpu_fetch(cpu);
			cpu->registers[reg] = literal;	
		} break;
		case MOV_REG_REG: {
			const u8 regFrom = cpu_fetch(cpu);
			const u8 regTo = cpu_fetch(cpu);
			cpu->registers[regFrom] = cpu->registers[regTo];
		} break;
		case MOV_REG_MEM: {
			const u8 reg = cpu_fetch(cpu);
			const u16 address = cpu_fetch16(cpu);
			cpu_memWrite16(cpu, address, cpu->registers[reg]);
		} break;
		case MOV_MEM_REG: {
			const u16 address = cpu_fetch16(cpu);
			const u8 reg = cpu_fetch(cpu);

			const u8 hi = cpu->memory[address];
			const u8 lo = cpu->memory[address + 1];
			cpu->registers[reg] = (u16) ((hi << 8) | (lo));
		} break;
		case ADD_REG_REG: {
			const u8 r1 = cpu_fetch(cpu);
			const u8 r2 = cpu_fetch(cpu);
			const u16 reg_value1 = cpu->registers[r1];
			const u16 reg_value2 = cpu->registers[r2];
			cpu->registers[CPU_REG_ACC] = reg_value1 + reg_value2;
		} break;
		case JNE_LIT: {
			const u16 value = cpu_fetch16(cpu);
			const u16 address = cpu_fetch16(cpu);

			if (value != cpu->registers[CPU_REG_ACC]) {
				cpu->registers[CPU_REG_IP] = address;
			}
		} break;
		case PSH_LIT: {
			const u16 value = cpu_fetch16(cpu);
			cpu_stackPush(cpu, value);
		} break;
		case PSH_REG: {
			const u8 register_index = cpu_fetch(cpu);
			cpu_stackPush(cpu, cpu->registers[register_index]);
		} break;
		case POP: {
			const u8 register_index = cpu_fetch(cpu);	
			cpu->registers[register_index] = cpu_stackPop(cpu);
		} break;
		case CAL_LIT: {
			const u16 address = cpu_fetch16(cpu);
			cpu_stackPushState(cpu);
			cpu->registers[CPU_REG_IP] = address;
		} break;
		case CAL_REG: {
			const u8 register_index = cpu_fetch(cpu);
			const u16 address = cpu->registers[register_index];
			cpu_stackPushState(cpu);
			cpu->registers[CPU_REG_IP] = address;
		} break;
		case RET: {
			cpu_stackPopState(cpu);	
		} break;
	}
}

void cpu_step(cpu_t *cpu) {
	u8 instruction = cpu_fetch(cpu);
	cpu_execute(cpu, instruction);
}
