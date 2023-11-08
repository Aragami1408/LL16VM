#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "mpc.h"
#include "../base_helper.h"

#include "instruction_parser.h"
#include "instruction_evaluator.h"
#include "utils.h"

int run_file(mpc_parser_t *parser, const char *filename) {

	int line_count;
	char **lines = read_file_to_lines(filename, &line_count);

	if (lines != NULL) {
		for (int i = 0; i < line_count; i++) {

			if (strcmp(lines[i], "") == 0) continue;
			printf("Line %d: %s\n", i + 1, lines[i]);
			parser_instruction_t *inst = evaluate_instruction(parser, "input", lines[i]);

			int machine_code[inst->type_size];	
			for (int i = 0; i < inst->type_size; i++) {
				machine_code[i] = 0x00;
			}

			generate_machine_code(inst, machine_code);
			printf("Machine Code: ");
			for (int i = 0; i < inst->type_size; i++) {
				if (machine_code[i] == -1) printf("failed ");
				else printf("0x%02x ", machine_code[i]);
			}

			printf("\n\n");

			free(lines[i]); // Free the dynamically allocated memory for each line
			free(inst);
		}

		free(lines);
	}

	return 0;
}

int main(int argc, char **argv) {

	mpc_parser_t *assembler = mpc_new("assembler");

	// Opcode parsers
	opcode_parsers_t *opc_parsers = opcode_parsers_init();	

	// Instruction type parsers
	inst_type_parsers_t *inst_type_parsers = inst_type_parsers_init();

	// Identifier parsers
	mpc_parser_t *hex_literal = mpc_new("hex_literal");
	mpc_parser_t *address = mpc_new("address");

	// Square bracket expression parsers
	mpc_parser_t *square_bracket_expr = mpc_new("square_bracket_expr");
	mpc_parser_t *expr = mpc_new("expr");
	mpc_parser_t *term = mpc_new("term");
	mpc_parser_t *factor = mpc_new("factor");

	mpc_parser_t *variable = mpc_new("variable");

	// Register parsers
	mpc_parser_t *registers = mpc_new("registers");
	mpc_parser_t *gp_registers = mpc_new("gp_registers");

	mpca_lang_contents(MPCA_LANG_DEFAULT, "assembler.grammar",
			assembler,

			opc_parsers->mov,
			opc_parsers->add,
			opc_parsers->sub,
			opc_parsers->mul,
			opc_parsers->lsf,
			opc_parsers->rsf,
			opc_parsers->and,
			opc_parsers->or,
			opc_parsers->xor,
			opc_parsers->inc,
			opc_parsers->dec,
			opc_parsers->not,
			opc_parsers->jeq,
			opc_parsers->jne,
			opc_parsers->jlt,
			opc_parsers->jgt,
			opc_parsers->jle,
			opc_parsers->jge,
			opc_parsers->psh,
			opc_parsers->pop,
			opc_parsers->cal,
			opc_parsers->ret,
			opc_parsers->hlt,

			inst_type_parsers->lit,
			inst_type_parsers->lit_reg,
			inst_type_parsers->reg_lit,
			inst_type_parsers->reg_reg,
			inst_type_parsers->reg_mem,
			inst_type_parsers->mem_reg,
			inst_type_parsers->lit_mem,
			inst_type_parsers->reg_ptr_reg,
			inst_type_parsers->lit_off_reg,

			hex_literal,
			address,

			square_bracket_expr,
			expr,
			term,
			factor,

			variable,

			registers,
			gp_registers
				);

	if (argc < 2) {
		printf("Missing argument: %s <asm_file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	else {
		run_file(assembler, argv[1]);
	}

	mpc_delete(assembler);

	opcode_parsers_free(opc_parsers);

	inst_type_parsers_free(inst_type_parsers);

	mpc_delete(hex_literal);
	mpc_delete(address);

	mpc_delete(square_bracket_expr);
	mpc_delete(expr);
	mpc_delete(term);
	mpc_delete(factor);

	mpc_delete(variable);

	mpc_delete(registers);
	mpc_delete(gp_registers);
}
