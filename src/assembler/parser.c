#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include "mpc.h"
#include "../base_helper.h"
#include "parser_instruction.h"
#include "str_utils.h"

int run_file(mpc_parser_t *parser, const char *filename) {

	FILE *file = fopen(filename, "r");

	if (file == NULL) {
		perror("Can't open file, going REPL mode\n");
		return 1;
	}

	char **lines = NULL;
	char buffer[256];
	size_t line_count = 0;

	while(fgets(buffer, sizeof(buffer), file) != NULL) {
		buffer[strcspn(buffer, "\n")] = '\0';

		char **temp = realloc(lines, (line_count + 1) * sizeof(char *));
		if (temp == NULL) {
			perror("Memory allocation error");
			fclose(file);
			free(lines); // Free previously allocated memory
			return 1;
		}

		lines = temp;

		lines[line_count] = strdup(buffer);
		trim_whitespace(lines[line_count]);
		line_count++;
	}

	fclose(file);

	if (line_count == 0) {
		printf("The file is empty.\n");
	} else {
		for (size_t i = 0; i < line_count; i++) {
			if (strcmp(lines[i], "") == 0) continue;
			printf("Line %zu: %s\n", i + 1, lines[i]);

			instruction_t *inst = parse_instruction(parser, "input", lines[i]);
			int machine_code[inst->type_size];	

			// for (int i = 0; i < inst->type_size; i++) {
			// 	machine_code[i] = 0x00;
			// }

			// generate_machine_code(inst, machine_code);
			// printf("Machine Code: ");
			// for (int i = 0; i < inst->type_size; i++) {
			// 	if (machine_code[i] == -1) printf("failed ");
			// 	else printf("0x%02x ", machine_code[i]);
			// }

			printf("\n\n");

			free(lines[i]); // Free the dynamically allocated memory for each line
			// free(inst);
		}
	}

	free(lines); // Free the array itself
	return 0;
}

int main(int argc, char **argv) {

	mpc_parser_t *assembler = mpc_new("assembler");

	mpc_instruction_t *mpc_inst = mpc_instruction_init();
	mpc_instruction_type_t *mpc_inst_type = mpc_instruction_type_init();

	mpc_parser_t *hex_literal = mpc_new("hex_literal");
	mpc_parser_t *address = mpc_new("address");

	mpc_parser_t *square_bracket_expr = mpc_new("square_bracket_expr");
	mpc_parser_t *expr = mpc_new("expr");
	mpc_parser_t *product = mpc_new("product");
	mpc_parser_t *value = mpc_new("value");

	mpc_parser_t *label = mpc_new("label");
	mpc_parser_t *variable = mpc_new("variable");

	mpc_parser_t *registers = mpc_new("registers");
	mpc_parser_t *gp_registers = mpc_new("gp_registers");

	mpca_lang_contents(MPCA_LANG_DEFAULT, "assembler.grammar",
					   assembler,

					   mpc_inst->mov,
					   mpc_inst->add,
					   mpc_inst->sub,
					   mpc_inst->mul,
					   mpc_inst->lsf,
					   mpc_inst->rsf,
					   mpc_inst->and,
					   mpc_inst->or,
					   mpc_inst->xor,
					   mpc_inst->inc,
					   mpc_inst->dec,
					   mpc_inst->not,
					   mpc_inst->jeq,
					   mpc_inst->jne,
					   mpc_inst->jlt,
					   mpc_inst->jgt,
					   mpc_inst->jle,
					   mpc_inst->jge,
					   mpc_inst->psh,
					   mpc_inst->pop,
					   mpc_inst->cal,
					   mpc_inst->ret,
					   mpc_inst->hlt,

					   mpc_inst_type->lit,
					   mpc_inst_type->lit_reg,
					   mpc_inst_type->reg_lit,
					   mpc_inst_type->reg_reg,
					   mpc_inst_type->reg_mem,
					   mpc_inst_type->mem_reg,
					   mpc_inst_type->lit_mem,
					   mpc_inst_type->reg_ptr_reg,
					   mpc_inst_type->lit_off_reg,

					   hex_literal,
					   address,

					   square_bracket_expr,
					   expr,
					   product,
					   value,

					   label,
					   variable,

					   registers,
					   gp_registers);

	if (argc < 2) {
		printf("Missing argument: %s <asm_file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	else {
		run_file(assembler, argv[1]);
	}

	mpc_delete(assembler);

	mpc_instruction_free(mpc_inst);
	mpc_instruction_type_free(mpc_inst_type);

	mpc_delete(hex_literal);
	mpc_delete(address);

	mpc_delete(square_bracket_expr);
	mpc_delete(expr);
	mpc_delete(product);
	mpc_delete(value);

	mpc_delete(variable);
	mpc_delete(label);

	mpc_delete(registers);
	mpc_delete(gp_registers);
}
