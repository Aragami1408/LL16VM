#ifndef ASSEMBLER_PARSER_INSTRUCTION_H
#define ASSEMBLER_PARSER_INSTRUCTION_H

#include "mpc.h"

typedef struct mpc_instruction {
	mpc_parser_t *mov;
	mpc_parser_t *add;
	mpc_parser_t *sub;
	mpc_parser_t *mul;
	mpc_parser_t *lsf;
	mpc_parser_t *rsf;
	mpc_parser_t *and;
	mpc_parser_t *or;
	mpc_parser_t *xor;
	mpc_parser_t *inc;
	mpc_parser_t *dec;
	mpc_parser_t *not;
	mpc_parser_t *jeq;
	mpc_parser_t *jne;
	mpc_parser_t *jlt;
	mpc_parser_t *jgt;
	mpc_parser_t *jle;
	mpc_parser_t *jge;
	mpc_parser_t *psh;
	mpc_parser_t *pop;
	mpc_parser_t *cal;
	mpc_parser_t *ret;
	mpc_parser_t *hlt;
} mpc_instruction_t;

typedef struct mpc_instruction_type {
	mpc_parser_t *lit;
	mpc_parser_t *lit_reg;
	mpc_parser_t *reg_lit;
	mpc_parser_t *reg_reg;
	mpc_parser_t *reg_mem;
	mpc_parser_t *mem_reg;
	mpc_parser_t *lit_mem;
	mpc_parser_t *reg_ptr_reg;
	mpc_parser_t *lit_off_reg;
} mpc_instruction_type_t;

mpc_instruction_t *mpc_instruction_init(void);
void mpc_instruction_free(mpc_instruction_t *mpc_inst);

mpc_instruction_type_t *mpc_instruction_type_init(void);
void mpc_instruction_type_free(mpc_instruction_type_t *mpc_inst_type);

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

#define MAX_LENGTH 100

typedef struct parser_instruction {
	char mnemonic[MAX_LENGTH];
	// char type[MAX_LENGTH]; // for debugging only
	instruction_type_t type;
	int type_size;
	union {
		struct {
			char lit[MAX_LENGTH];
			char reg[MAX_LENGTH];
		} lit_reg;

		struct {
			char reg1[MAX_LENGTH];
			char reg2[MAX_LENGTH];
		} reg_reg;

		struct {
			char reg[MAX_LENGTH];
			char mem[MAX_LENGTH];
		} reg_mem;

		struct {
			char lit[MAX_LENGTH];
			char mem[MAX_LENGTH];

		} lit_mem;

		struct {
			char lit[MAX_LENGTH];
			char reg1[MAX_LENGTH];
			char reg2[MAX_LENGTH];
		} lit_off_reg;

		char single_reg[MAX_LENGTH];
		char single_lit[MAX_LENGTH];	
	};
} instruction_t;


instruction_t *parse_instruction(mpc_parser_t *parser, const char *filename, const char *input);
void generate_machine_code(instruction_t *inst, int *machine_code);

#endif