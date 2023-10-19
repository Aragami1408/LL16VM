#ifndef CPU_H
#define CPU_H

#include "base_types.h"
#include "memory_mapper.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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
	CPU_REG_SP,
	CPU_REG_FP,
	CPU_REG_COUNT
} cpu_registers_t;

typedef struct {
	memmapper_t *memory;
	u16 registers[CPU_REG_COUNT];	
	u16 stackframe_size;
	bool is_halt;	
} cpu_t;

cpu_t *cpu_init(memmapper_t *memory);
void cpu_free(cpu_t *cpu);

void cpu_debug(cpu_t *cpu);
void cpu_viewMemoryAt(cpu_t *cpu, u16 address);

u8 cpu_fetch(cpu_t *cpu);
u16 cpu_fetch16(cpu_t *cpu);

void cpu_stackPush(cpu_t *cpu, u16 value);
void cpu_stackPushState(cpu_t *cpu);
u16 cpu_stackPop(cpu_t *cpu);
void cpu_stackPopState(cpu_t *cpu);

void cpu_execute(cpu_t *cpu, u8 instruction);
void cpu_step(cpu_t *cpu);
void cpu_run(cpu_t *cpu);

#endif
