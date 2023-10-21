#include "cpu.h"
#include "base_helper.h"
#include "instructions.h"


cpu_t *cpu_init(memmapper_t *memory) {
	cpu_t *cpu = (cpu_t *) malloc(sizeof(cpu_t));
	
	if(!cpu) {
		exit(EXIT_FAILURE);
	}
	
	for(int i = 0; i < CPU_REG_COUNT; i++) {
		cpu->registers[i] = 0;
	}
	
	// Loading instructions into the memory
	// for(int i = 0; i < nbytes; i++) {
	// 	cpu->memory[i] = writable_bytes[i];
	// }
	cpu->memory = memory;
	
	// cpu->registers[CPU_REG_SP] = ArrayCount(cpu->memory) - 1 - 1;
	// cpu->registers[CPU_REG_FP] = ArrayCount(cpu->memory) - 1 - 1;
	cpu->registers[CPU_REG_SP] = 0xffff - 1;
	cpu->registers[CPU_REG_FP] = 0xffff - 1;
	
	cpu->stackframe_size = 0;
	cpu->is_halt = false;
	
	return cpu;
}

void cpu_free(cpu_t *cpu) {
	// free(cpu->memory);
	// free(cpu->registers);
	free(cpu);
}

u8 cpu_fetch(cpu_t *cpu) {
	const u16 next_instruction_address = cpu->registers[CPU_REG_IP];
	// const u8 instruction = cpu->memory[next_instruction_address];
	const u8 instruction = memmapper_readU8(cpu->memory, next_instruction_address);
	cpu->registers[CPU_REG_IP] = next_instruction_address + 1;
	return instruction;
}

u16 cpu_fetch16(cpu_t *cpu) {
	const u16 next_instruction_address = cpu->registers[CPU_REG_IP];
	// const u16 ins_hi = cpu->memory[next_instruction_address];
	// const u16 ins_lo = cpu->memory[next_instruction_address + 1];
	const u8 ins_hi = memmapper_readU8(cpu->memory, next_instruction_address);
	const u8 ins_lo = memmapper_readU8(cpu->memory, next_instruction_address + 1);
	
	cpu->registers[CPU_REG_IP] = next_instruction_address + 2;
	return (u16) ((ins_hi << 8) | (ins_lo));
}

void cpu_stackPush(cpu_t *cpu, u16 value) {
	const u16 sp_address = cpu->registers[CPU_REG_SP];
	// cpu_memWrite16(cpu, sp_address, value);
	memmapper_writeU16(cpu->memory, sp_address, value);
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
	return memmapper_readU16(cpu->memory, next_sp_address);
}

void cpu_stackPopState(cpu_t *cpu) {
	const u16 fp_address = cpu->registers[CPU_REG_FP];
	cpu->registers[CPU_REG_SP] = fp_address;
	
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
			// cpu_memWrite16(cpu, address, cpu->registers[reg]);
			memmapper_writeU16(cpu->memory, address, cpu->registers[reg]);
		} break;
		case MOV_MEM_REG: {
			const u16 address = cpu_fetch16(cpu);
			const u8 reg = cpu_fetch(cpu);
			
			// const u8 hi = cpu->memory[address];
			// const u8 lo = cpu->memory[address + 1];
			const u8 hi = memmapper_readU8(cpu->memory, address);
			const u8 lo = memmapper_readU8(cpu->memory, address + 1);
			cpu->registers[reg] = (u16) ((hi << 8) | (lo));
		} break;
		case MOV_LIT_MEM: {
			const u16 value = cpu_fetch16(cpu);
			const u16 address = cpu_fetch16(cpu);
			memmapper_writeU16(cpu->memory, address, value);
		} break;
		case MOV_REG_PTR_REG: {
			const u8 r1 = cpu_fetch(cpu);
			const u8 r2 = cpu_fetch(cpu);
			const u16 ptr = cpu->registers[r1];
			const u16 value = memmapper_readU16(cpu->memory, ptr);
			cpu->registers[r2] = value;
		} break;
		case MOV_LIT_OFF_REG: {
			const u16 base_address = cpu_fetch16(cpu);
			const u8 r1 = cpu_fetch(cpu);
			const u8 r2 = cpu_fetch(cpu);
			const u16 offset = cpu->registers[r1];
			
			const u16 value = memmapper_readU16(cpu->memory, base_address + offset);
			cpu->registers[r2] = value;
		} break;
		
		case ADD_REG_REG: {
			const u8 r1 = cpu_fetch(cpu);
			const u8 r2 = cpu_fetch(cpu);
			const u16 reg_value1 = cpu->registers[r1];
			const u16 reg_value2 = cpu->registers[r2];
			cpu->registers[CPU_REG_ACC] = reg_value1 + reg_value2;
		} break;
		case ADD_LIT_REG: {
			const u16 literal = cpu_fetch16(cpu);
			const u8 r1 = cpu_fetch(cpu);
			const u16 reg_value = cpu->registers[r1];
			cpu->registers[CPU_REG_ACC] = literal + reg_value;
		} break;
		case SUB_LIT_REG: {
			const u16 literal = cpu_fetch16(cpu);
			const u8 r1 = cpu_fetch(cpu);
			const u16 reg_value = cpu->registers[r1];
			cpu->registers[CPU_REG_ACC] = reg_value - literal;
		} break;
		case SUB_REG_LIT: {
			const u8 r1 = cpu_fetch(cpu);
			const u16 literal = cpu_fetch16(cpu);
			const u16 reg_value = cpu->registers[r1];
			cpu->registers[CPU_REG_ACC] = literal - reg_value;
		} break;
		case SUB_REG_REG: {
			const u8 r1 = cpu_fetch(cpu);
			const u8 r2 = cpu_fetch(cpu);
			const u16 reg_value1 = cpu->registers[r1];
			const u16 reg_value2 = cpu->registers[r2];
			cpu->registers[CPU_REG_ACC] = reg_value1 - reg_value2;
		} break;
		case MUL_LIT_REG: {
			const u16 literal = cpu_fetch16(cpu);
			const u8 r1 = cpu_fetch(cpu);
			const u16 reg_value = cpu->registers[r1];
			cpu->registers[CPU_REG_ACC] = literal * reg_value;
		} break;
		case MUL_REG_REG: {
			const u8 r1 = cpu_fetch(cpu);
			const u8 r2 = cpu_fetch(cpu);
			const u16 reg_value1 = cpu->registers[r1];
			const u16 reg_value2 = cpu->registers[r2];
			cpu->registers[CPU_REG_ACC] = reg_value1 * reg_value2;
		} break;
		case INC_REG: {
			const u8 r1 = cpu_fetch(cpu);
			const u16 old_value = cpu->registers[r1];
			const u16 new_value = old_value + 1;
			cpu->registers[r1] = new_value;
		} break;
		case DEC_REG: {
			const u8 r1 = cpu_fetch(cpu);
			const u16 old_value = cpu->registers[r1];
			const u16 new_value = old_value - 1;
			cpu->registers[r1] = new_value;
		} break;
		
		case LSF_REG_LIT: {
			const u8 r1 = cpu_fetch(cpu);
			const u16 literal = cpu_fetch16(cpu);
			const u16 reg_value = cpu->registers[r1];
			cpu->registers[r1] = reg_value << literal;
		} break;
		case LSF_REG_REG: {
			const u8 r1 = cpu_fetch(cpu);
			const u8 r2 = cpu_fetch(cpu);
			const u16 reg_value = cpu->registers[r1];
			const u16 shift_by = cpu->registers[r2];
			cpu->registers[r1] = reg_value << shift_by;
		} break;
		case RSF_REG_LIT: {
			const u8 r1 = cpu_fetch(cpu);
			const u16 literal = cpu_fetch16(cpu);
			const u16 reg_value = cpu->registers[r1];
			cpu->registers[r1] = reg_value >> literal;
		} break;
		case RSF_REG_REG: {
			const u8 r1 = cpu_fetch(cpu);
			const u8 r2 = cpu_fetch(cpu);
			const u16 reg_value = cpu->registers[r1];
			const u16 shift_by = cpu->registers[r2];
			cpu->registers[r1] = reg_value >> shift_by;
		} break;
		case AND_REG_LIT: {
			const u8 r1 = cpu_fetch(cpu);
			const u16 literal = cpu_fetch16(cpu);
			const u16 reg_value = cpu->registers[r1];
			cpu->registers[CPU_REG_ACC] = reg_value & literal;
		} break;
		case AND_REG_REG: {
			const u8 r1 = cpu_fetch(cpu);
			const u8 r2 = cpu_fetch(cpu);
			const u16 reg_value = cpu->registers[r1];
			const u16 shift_by = cpu->registers[r2];
			cpu->registers[CPU_REG_ACC] = reg_value & shift_by;
		} break;
		case OR_REG_LIT: {
			const u8 r1 = cpu_fetch(cpu);
			const u16 literal = cpu_fetch16(cpu);
			const u16 reg_value = cpu->registers[r1];
			cpu->registers[CPU_REG_ACC] = reg_value | literal;
		} break;
		case OR_REG_REG: {
			const u8 r1 = cpu_fetch(cpu);
			const u8 r2 = cpu_fetch(cpu);
			const u16 reg_value = cpu->registers[r1];
			const u16 shift_by = cpu->registers[r2];
			cpu->registers[CPU_REG_ACC] = reg_value | shift_by;
		} break;
		case XOR_REG_LIT: {
			const u8 r1 = cpu_fetch(cpu);
			const u16 literal = cpu_fetch16(cpu);
			const u16 reg_value = cpu->registers[r1];
			cpu->registers[CPU_REG_ACC] = reg_value ^ literal;
		} break;
		case XOR_REG_REG: {
			const u8 r1 = cpu_fetch(cpu);
			const u8 r2 = cpu_fetch(cpu);
			const u16 reg_value = cpu->registers[r1];
			const u16 shift_by = cpu->registers[r2];
			cpu->registers[CPU_REG_ACC] = reg_value ^ shift_by;
		} break;
		case NOT: {
			const u8 r1 = cpu_fetch(cpu);
			const u16 reg_value = cpu->registers[r1];
			
			const u16 res = (~reg_value) & 0xffff;
			cpu->registers[CPU_REG_ACC] = res;
		} break;
		
		case JNE_LIT: {
			const u16 value = cpu_fetch16(cpu);
			const u16 address = cpu_fetch16(cpu);
			
			if (value != cpu->registers[CPU_REG_ACC]) {
				cpu->registers[CPU_REG_IP] = address;
			}
		} break;
		case JNE_REG: {
			const u8 r1 = cpu_fetch(cpu);
			const u16 value = cpu->registers[r1];
			const u16 address = cpu_fetch16(cpu);
			
			if(value != cpu->registers[CPU_REG_ACC]) {
				cpu->registers[CPU_REG_IP] = address;
			}
		} break;
		case JEQ_LIT: {
			const u16 value = cpu_fetch16(cpu);
			const u16 address = cpu_fetch16(cpu);
			
			if (value == cpu->registers[CPU_REG_ACC]) {
				cpu->registers[CPU_REG_IP] = address;
			}
		} break;
		case JEQ_REG: {
			const u8 r1 = cpu_fetch(cpu);
			const u16 value = cpu->registers[r1];
			const u16 address = cpu_fetch16(cpu);
			
			if(value == cpu->registers[CPU_REG_ACC]) {
				cpu->registers[CPU_REG_IP] = address;
			}
		} break;
		case JLT_LIT: {
			const u16 value = cpu_fetch16(cpu);
			const u16 address = cpu_fetch16(cpu);
			
			if (value < cpu->registers[CPU_REG_ACC]) {
				cpu->registers[CPU_REG_IP] = address;
			}
		} break;
		case JLT_REG: {
			const u8 r1 = cpu_fetch(cpu);
			const u16 value = cpu->registers[r1];
			const u16 address = cpu_fetch16(cpu);
			
			if(value < cpu->registers[CPU_REG_ACC]) {
				cpu->registers[CPU_REG_IP] = address;
			}
		} break;
		case JGT_LIT: {
			const u16 value = cpu_fetch16(cpu);
			const u16 address = cpu_fetch16(cpu);
			
			if (value > cpu->registers[CPU_REG_ACC]) {
				cpu->registers[CPU_REG_IP] = address;
			}
		} break;
		case JGT_REG: {
			const u8 r1 = cpu_fetch(cpu);
			const u16 value = cpu->registers[r1];
			const u16 address = cpu_fetch16(cpu);
			
			if(value > cpu->registers[CPU_REG_ACC]) {
				cpu->registers[CPU_REG_IP] = address;
			}
		} break;
		case JLE_LIT: {
			const u16 value = cpu_fetch16(cpu);
			const u16 address = cpu_fetch16(cpu);
			
			if (value <= cpu->registers[CPU_REG_ACC]) {
				cpu->registers[CPU_REG_IP] = address;
			}
		} break;
		case JLE_REG: {
			const u8 r1 = cpu_fetch(cpu);
			const u16 value = cpu->registers[r1];
			const u16 address = cpu_fetch16(cpu);
			
			if(value <= cpu->registers[CPU_REG_ACC]) {
				cpu->registers[CPU_REG_IP] = address;
			}
		} break;
		case JGE_LIT: {
			const u16 value = cpu_fetch16(cpu);
			const u16 address = cpu_fetch16(cpu);
			
			if (value >= cpu->registers[CPU_REG_ACC]) {
				cpu->registers[CPU_REG_IP] = address;
			}
		} break;
		case JGE_REG: {
			const u8 r1 = cpu_fetch(cpu);
			const u16 value = cpu->registers[r1];
			const u16 address = cpu_fetch16(cpu);
			
			if(value >= cpu->registers[CPU_REG_ACC]) {
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
		
		case HLT: {
			cpu->is_halt = true;
		} break;
	}
}

void cpu_step(cpu_t *cpu) {
	u8 instruction = cpu_fetch(cpu);
	return cpu_execute(cpu, instruction);
}

void cpu_run(cpu_t *cpu) {
	cpu_step(cpu);
	if (!cpu->is_halt) {
		cpu_run(cpu);	
	}	
}
