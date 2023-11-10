#ifndef INSTRUCTION_PARSER_H
#define INSTRUCTION_PARSER_H

#include <stdlib.h>

#include "mpc.h"

typedef struct opcode_parsers {
    
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
} opcode_parsers_t;

typedef struct inst_type_parsers {
	mpc_parser_t *lit;
	mpc_parser_t *lit_reg;
	mpc_parser_t *reg_lit;
	mpc_parser_t *reg_reg;
	mpc_parser_t *reg_mem;
	mpc_parser_t *mem_reg;
	mpc_parser_t *lit_mem;
	mpc_parser_t *reg_ptr_reg;
	mpc_parser_t *lit_off_reg;
} inst_type_parsers_t;

opcode_parsers_t *opcode_parsers_init(void);
void opcode_parsers_free(opcode_parsers_t *opc);

inst_type_parsers_t *inst_type_parsers_init(void);
void inst_type_parsers_free(inst_type_parsers_t *inst_type);


#endif
