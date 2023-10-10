#ifndef CPU_H
#define CPU_H

#include "base_types.h"
#include "instructions.h"

#include <stdio.h>
#include <stdlib.h>

typedef enum {
	CPU_REG_IP = 0,
	CPU_REG_ACC,
	CPU_REG_R1,
	CPU_REG_R2,
	CPU_REG_R3,
	CPU_REG_R4,
	CPU_REG_R5,
	CPU_REG_R6,
	CPU_REG_R7,
	CPU_REG_R8,
	CPU_REG_COUNT
} cpu_registers_t;

typedef struct {
	u8 memory[256*256];
	u16 registers[CPU_REG_COUNT];	
} cpu_t;

cpu_t *cpu_init(u8 *writable_bytes, int nbytes);

void cpu_debug(cpu_t *cpu);
void cpu_viewMemoryAt(cpu_t *cpu, u16 address);

u8 cpu_fetch(cpu_t *cpu);
u16 cpu_fetch16(cpu_t *cpu);

u16 cpu_memRead16(cpu_t *cpu, u16 address);
void cpu_memWrite16(cpu_t *cpu, u16 address, u16 data);

void cpu_execute(cpu_t *cpu, u8 instruction);
void cpu_step(cpu_t *cpu);

#endif
