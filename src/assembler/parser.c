#include "mpc.h"

#ifdef _WIN32
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

int main() {
	
	mpc_parser_t *assembler = mpc_new("assembler");

	mpc_parser_t *mov = mpc_new("mov");
	mpc_parser_t *add = mpc_new("add");
	mpc_parser_t *sub = mpc_new("sub");
	mpc_parser_t *mul = mpc_new("mul");
	mpc_parser_t *lsf = mpc_new("lsf");
	mpc_parser_t *rsf = mpc_new("rsf");
	mpc_parser_t *and = mpc_new("and");
	mpc_parser_t *or = mpc_new("or");
	mpc_parser_t *xor = mpc_new("xor");
	mpc_parser_t *inc = mpc_new("inc");
	mpc_parser_t *dec = mpc_new("dec");
	mpc_parser_t *not = mpc_new("not");

	mpc_parser_t *lit= mpc_new("lit");
	mpc_parser_t *lit_reg = mpc_new("lit_reg");
	mpc_parser_t *reg_reg = mpc_new("reg_reg");
	mpc_parser_t *reg_mem = mpc_new("reg_mem");
	mpc_parser_t *mem_reg = mpc_new("mem_reg");
	mpc_parser_t *lit_mem = mpc_new("lit_mem");
	mpc_parser_t *reg_ptr_reg = mpc_new("reg_ptr_reg");
	mpc_parser_t *lit_off_reg = mpc_new("lit_off_reg");

	mpc_parser_t *hex_literal = mpc_new("hex_literal");
	mpc_parser_t *address = mpc_new("address");

	mpc_parser_t *square_bracket_expr = mpc_new("square_bracket_expr");
	mpc_parser_t *expr = mpc_new("expr");
	mpc_parser_t *term = mpc_new("term");
	mpc_parser_t *factor = mpc_new("factor");

	mpc_parser_t *variable = mpc_new("variable");

	mpc_parser_t *registers = mpc_new("registers");
	mpc_parser_t *gp_registers = mpc_new("gp_registers");

	mpca_lang_contents(MPCA_LANG_DEFAULT, "assembler.grammar",
					assembler,

					mov,
					add,
					sub,
					mul,
					lsf,
					rsf,
					and,
					or,
					xor,
					inc,
					dec,
					not,

					lit,
					lit_reg,
					reg_reg,
					reg_mem,
					mem_reg,
					lit_mem,
					reg_ptr_reg,
					lit_off_reg,

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

	while (1) {
		char *input = readline("$ ");
		add_history(input);

		mpc_result_t r;
		if (mpc_parse("stdin", input, assembler, &r)) {
			mpc_ast_print(r.output);
			mpc_ast_delete(r.output);
		}
		else {
		mpc_err_print(r.error);
			mpc_err_delete(r.error);
		}

		free(input);
	}	
		
	mpc_delete(assembler);

	mpc_delete(mov);
	mpc_delete(add);
	mpc_delete(sub);
	mpc_delete(mul);
	mpc_delete(lsf);
	mpc_delete(rsf);
	mpc_delete(and);
	mpc_delete(or);
	mpc_delete(xor);
	mpc_delete(inc);
	mpc_delete(dec);
	mpc_delete(not);

	mpc_delete(lit);
	mpc_delete(lit_reg);
	mpc_delete(reg_reg);
	mpc_delete(reg_mem);
	mpc_delete(mem_reg);
	mpc_delete(lit_mem);
	mpc_delete(reg_ptr_reg);
	mpc_delete(lit_off_reg);

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
