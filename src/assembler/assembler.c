#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "mpc.h"
#include "../base_helper.h"
#include "../base_defines.h"

#include "utils.h"

#include "instruction_parser.h"
#include "instruction_evaluator.h"

#include "expr_parser.h"
#include "expr_evaluator.h"

#include "common_parser.h"

#if OS_WINDOWS == 1

static char buffer[2048];

char* readline(char* prompt) {
	fputs(prompt, stdout);
	fgets(buffer, 2048, stdin);
	char* cpy = malloc(strlen(buffer)+1);
	strcpy(cpy, buffer);
	cpy[strlen(cpy)-1] = '\0';
	return cpy;
}

void add_history(char* unused) {}

#else
#include <editline/readline.h>
#endif

// FOR DEBUGGING ONLY
int run_repl_no_eval(mpc_parser_t *parser) {
	while (1) {
		char *input = readline("$ ");
		add_history(input);

		mpc_result_t r;
		if (mpc_parse("<stdin", input, parser, &r)) {
			mpc_ast_print(r.output);
			mpc_ast_delete(r.output);
		}
		else {
			mpc_err_print(r.error);
			mpc_err_delete(r.error);
		}
	}
	return 0;
}

// FOR DEBUGGING ONLY
int run_repl_expr(mpc_parser_t *parser) {
	while (1) {
		char *input = readline("$ ");
		add_history(input);

		mpc_result_t r;
		if (mpc_parse("<stdin>", input, parser, &r)) {
			mpc_ast_t *ast = r.output;
			int result = evaluate_expression(ast);
			printf("The result of the expression is: $%04x\n", result);	
			mpc_ast_delete(ast);
		}
		else {
			mpc_err_print(r.error);
			mpc_err_print(r.error);
		}
	}
	return 0;
}

int run_repl(mpc_parser_t *parser) {
	u16 current_address = 0x0000;
	while (1) {
		char *input = readline("$ ");
		add_history(input);

		
		parser_instruction_t *inst = evaluate_instruction(parser, "<stdin>", input);

		int machine_code[inst->type_size];	
		for (int i = 0; i < inst->type_size; i++) {
			machine_code[i] = 0x00;
		}

		generate_machine_code(inst, machine_code);
		printf("Machine Code: %04x: ", current_address);
		for (int i = 0; i < inst->type_size; i++) {
			if (machine_code[i] == -1) printf("failed ");
			else printf("$%02x ", machine_code[i]);
		}

		current_address += inst->type_size;

		printf("\n\n");

		free(inst);
	}
	return 0;
}

int run_file(mpc_parser_t *parser, const char *filename) {

	int line_count;
	char **lines = read_file_to_lines(filename, &line_count);

	if (lines != NULL) {
		u16 current_address = 0x0000;
		for (int i = 0; i < line_count; i++) {
			
			if (strcmp(lines[i], "") == 0) continue;
			trim_whitespace(lines[i]);
			printf("Line %d: %s\n", i + 1, lines[i]);

			parser_instruction_t *inst = evaluate_instruction(parser, "input", lines[i]);

			int machine_code[inst->type_size];	
			for (int i = 0; i < inst->type_size; i++) {
				machine_code[i] = 0x00;
			}

			generate_machine_code(inst, machine_code);
			printf("Machine Code: %04x: ", current_address);
			for (int i = 0; i < inst->type_size; i++) {
				if (machine_code[i] == -1) printf("failed ");
				else printf("0x%02x ", machine_code[i]);
			}

			current_address += inst->type_size;

			printf("\n\n");

			free(lines[i]);
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

	// Common parsers
	common_parsers_t *common_parsers = common_parsers_init();

	// Square bracket expression parsers
	expr_parser_t *expr_parser = expr_parser_init();	

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

			common_parsers->hex_literal,
			common_parsers->address,

			expr_parser->square_bracket_expr,
			expr_parser->expr,
			expr_parser->term,
			expr_parser->factor,

			common_parsers->variable,

			common_parsers->registers,
			common_parsers->gp_registers
				);

	if (argc < 2) {
		printf("Missing file. Going REPL mode\n");
		run_repl(assembler);
		// run_repl_no_eval(assembler);
		// run_repl_expr(expr_parser->square_bracket_expr);
	}
	else {
		run_file(assembler, argv[1]);
	}

	mpc_delete(assembler);

	opcode_parsers_free(opc_parsers);
	inst_type_parsers_free(inst_type_parsers);
	common_parsers_free(common_parsers);
	expr_parser_free(expr_parser);


}
