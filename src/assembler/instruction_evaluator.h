#ifndef INSTRUCTION_EVALUATOR_H
#define INSTRUCTION_EVALUATOR_H

#include "mpc.h"
#include "expr_evaluator.h"


#define MAX_INST_LENGTH 100

typedef enum {
	LIT_REG,
	REG_LIT,
	REG_LIT8,
	REG_REG,
	REG_MEM,
	MEM_REG,
	LIT_MEM,
	REG_PTR_REG,
	LIT_OFF_REG,
	NO_ARGS,
	SINGLE_REG,
	SINGLE_LIT,
} instruction_type_t;

typedef struct parser_instruction {
	char mnemonic[MAX_INST_LENGTH];
	// char type[MAX_INST_LENGTH]; // for debugging only
	instruction_type_t type;
	int type_size;
	union {
		struct {
			char lit[MAX_INST_LENGTH];
			char reg[MAX_INST_LENGTH];
		} lit_reg;

		struct {
			char reg1[MAX_INST_LENGTH];
			char reg2[MAX_INST_LENGTH];
		} reg_reg;

		struct {
			char reg[MAX_INST_LENGTH];
			char mem[MAX_INST_LENGTH];
		} reg_mem;

		struct {
			char lit[MAX_INST_LENGTH];
			char mem[MAX_INST_LENGTH];

		} lit_mem;

		struct {
			char lit[MAX_INST_LENGTH];
			char reg1[MAX_INST_LENGTH];
			char reg2[MAX_INST_LENGTH];
		} lit_off_reg;

		char single_reg[MAX_INST_LENGTH];
		char single_lit[MAX_INST_LENGTH];	
	};
} parser_instruction_t;

parser_instruction_t *evaluate_instruction(mpc_parser_t *parser, const char *filename, const char *input, const label_hashmap_t *label_map);

void generate_machine_code(parser_instruction_t *inst, int *machine_code);

#endif
