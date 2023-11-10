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

#include "label_evaluator.h"

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
		if (mpc_parse("<stdin>", input, parser, &r)) {
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

		label_hashmap_t label_map;
		init_label_hashmap(&label_map);
		insert_label(&label_map, "start", 0x00);
		insert_label(&label_map, "loop", 0x05);
		insert_label(&label_map, "end", 0x1a);

		mpc_result_t r;
		if (mpc_parse("<stdin>", input, parser, &r)) {
			mpc_ast_t *ast = r.output;
			printf("---------------------\n");
			mpc_ast_print(ast);
			printf("---------------------\n");
			token_list_t *infix_tokens = create_token_list(INITIAL_CAPACITY);
			dfs_traversal(ast, infix_tokens, &label_map);
			printf("---------------------\n");
			print_tokens(infix_tokens);
			printf("---------------------\n");
			token_list_t *rpn_tokens = infix_to_rpn(infix_tokens);
			print_tokens(rpn_tokens);
			printf("---------------------\n");
			int result = evaluate_postfix(rpn_tokens);
			printf("Expr result is: $%04x\n", result);


			free_token_list(rpn_tokens);
			free_token_list(infix_tokens);
			mpc_ast_delete(ast);
		}
		else {
			mpc_err_print(r.error);
			mpc_err_print(r.error);
		}
	}
	return 0;
}

label_hashmap_t scan_labels(mpc_parser_t *parser, const char *filename) {
	int line_count;
	char **lines = read_file_to_lines(filename, &line_count);

	label_hashmap_t label_map;
	init_label_hashmap(&label_map);

	if (lines != NULL) {
		u16 current_address = 0x0000;
		for (int i = 0; i < line_count; i++) {
			trim_whitespace(lines[i]);
			if (strcmp(lines[i], "") == 0) continue;
			
			mpc_result_t r;
			if (mpc_parse("input", lines[i], parser, &r)) {
				mpc_ast_t *ast = r.output;
				if (strstr(lines[i], "ret") || strstr(lines[i], "hlt")) {
				}
				else {
					mpc_ast_t *label_or_inst = ast->children[0];

					if (strstr(label_or_inst->tag, "label")) {
						insert_label(&label_map, label_or_inst->children[0]->contents, current_address);
					}
					else {
						mpc_ast_t *instruction_type = label_or_inst->children[1];

#define MATCH_TAG_AND_INCREASE(str, offset) if (strstr(instruction_type->tag, (str))) current_address += (offset);

						MATCH_TAG_AND_INCREASE("registers", 2)
						else MATCH_TAG_AND_INCREASE("lit|hex_literal", 3)
						else MATCH_TAG_AND_INCREASE("square_bracket_expr", 3)
						else {
							MATCH_TAG_AND_INCREASE("lit_reg", 4)
							else MATCH_TAG_AND_INCREASE("reg_reg", 3)
							else MATCH_TAG_AND_INCREASE("reg_reg", 3)
							else if (strstr(instruction_type->tag, "reg_lit")) {	
								if (strstr(label_or_inst->tag, "lsf") || strstr(label_or_inst->tag, "rsf")) {
									current_address += 3;
								} else {
									current_address += 4;
								}
							}
							else MATCH_TAG_AND_INCREASE("reg_mem", 4)
							else MATCH_TAG_AND_INCREASE("mem_reg", 4)
							else MATCH_TAG_AND_INCREASE("lit_mem", 5)
							else MATCH_TAG_AND_INCREASE("reg_ptr_reg", 3)
							else MATCH_TAG_AND_INCREASE("lit_off_reg", 5)
						}
					}
					
				}
				mpc_ast_delete(ast);
			}
			else {
				mpc_err_print(r.error);
				mpc_err_delete(r.error);
			}
		}
	}

	return label_map;
}

int run_file(mpc_parser_t *parser, const char *filename, label_hashmap_t *label_map) {

	int line_count;
	char **lines = read_file_to_lines(filename, &line_count);

	if (lines != NULL) {
		for (int i = 0; i < line_count; i++) {
			
			trim_whitespace(lines[i]);
			if (strcmp(lines[i], "") == 0) continue;

			parser_instruction_t *inst = evaluate_instruction(parser, "input", lines[i], label_map);

			if (inst != NULL) {
				int machine_code[inst->type_size];	
				for (int i = 0; i < inst->type_size; i++) {
					machine_code[i] = 0x00;
				}
				generate_machine_code(inst, machine_code);
				for (int i = 0; i < inst->type_size; i++) {
					if (machine_code[i] == -1) printf("failed ");
					else printf("%02x ", machine_code[i]);
				}
			}

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
			common_parsers->label,

			common_parsers->registers,
			common_parsers->gp_registers
				);

	if (argc < 2) {
		printf("Missing file. Going REPL mode\n");
		// run_repl(assembler);
		// run_repl_no_eval(assembler);
		run_repl_expr(expr_parser->square_bracket_expr);
	}
	else {
		label_hashmap_t label_map = scan_labels(assembler, argv[1]);
		run_file(assembler, argv[1], &label_map);
	}

	mpc_delete(assembler);

	opcode_parsers_free(opc_parsers);
	inst_type_parsers_free(inst_type_parsers);
	common_parsers_free(common_parsers);
	expr_parser_free(expr_parser);


}
