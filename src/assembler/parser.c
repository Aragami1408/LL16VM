#include "mpc.h"
#include "../base_helper.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

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
} parser_instruction_t;

parser_instruction_t *run(mpc_parser_t *parser, const char *filename, const char *input) {
	mpc_result_t r;
	parser_instruction_t *inst = (parser_instruction_t *) malloc(sizeof(parser_instruction_t));	

	if(mpc_parse(filename, input, parser, &r)) {
		mpc_ast_t *ast = r.output;
		printf("------------------------\n");
		if (strstr(input, "ret") || strstr(input, "hlt")) {
			strcpy(inst->mnemonic, ast->tag);
			inst->type = NO_ARGS;
			inst->type_size = 1;
			printf("Mnemonic: %s\n", inst->mnemonic);
			printf("Instruction Type: no_args\n");	
			printf("Instruction Size: %d\n", inst->type_size);
		}
		else {
			mpc_ast_t *mnemonic = ast->children[0];
			strcpy(inst->mnemonic, mnemonic->tag);
			printf("Mnemonic: %s\n", inst->mnemonic);

			mpc_ast_t *instruction_type = mnemonic->children[1];
			if (strstr(instruction_type->tag, "registers")) {
				mpc_ast_t *reg = instruction_type;
				
				// strcpy(inst->type, "single_reg|>");
				inst->type = SINGLE_REG;
				printf("Instruction Type: single_reg\n");

				inst->type_size = 2;

				strcpy(inst->single_reg, reg->contents);
				printf("\t Register: %s\n", inst->single_reg);
			}
			else if (strstr(instruction_type->tag, "lit|hex_literal")) {
				mpc_ast_t *lit = instruction_type;

				// strcpy(inst->type, "single_lit|>");
				inst->type = SINGLE_LIT;
				printf("Instruction Type: single_lit\n");

				inst->type_size = 3;
				printf("Instruction Size: %d\n", inst->type_size);

				strcpy(inst->single_lit, lit->children[1]->contents);
				printf("\t Literal: $%s\n", inst->single_lit);
			}
			else {
				
				printf("Instruction Type: %s\n", instruction_type->tag);
				// strcpy(inst->type, instruction_type->tag);
				if (strstr(instruction_type->tag, "lit_reg")) {
					mpc_ast_t *lit = instruction_type->children[0];	
					mpc_ast_t *reg = instruction_type->children[2];

					inst->type = LIT_REG;
					inst->type_size = 4;

					strcpy(inst->lit_reg.lit, lit->children[1]->contents);
					printf("\t Literal: $%s\n", inst->lit_reg.lit);


					strcpy(inst->lit_reg.reg, reg->contents);
					printf("\t Register: %s\n", inst->lit_reg.reg);
				}
				else if(strstr(instruction_type->tag, "reg_reg")) {
					mpc_ast_t *reg1 = instruction_type->children[0];	
					mpc_ast_t *reg2 = instruction_type->children[2];

					inst->type = REG_REG;
					inst->type_size = 3;

					strcpy(inst->reg_reg.reg1, reg1->contents);
					printf("\t Register 1: %s\n", inst->reg_reg.reg1);

					strcpy(inst->reg_reg.reg2, reg2->contents);
					printf("\t Register 2: %s\n", inst->reg_reg.reg2);
				
				}
				else if (strstr(instruction_type->tag, "reg_lit")) {
					mpc_ast_t *reg = instruction_type->children[0];	
					mpc_ast_t *lit = instruction_type->children[2];
					
					if (strlen(lit->children[1]->contents) <= 2) {
						inst->type = REG_LIT8;
						inst->type_size = 3;
					} else {
						inst->type = REG_LIT;
						inst->type_size = 4;
					}
					strcpy(inst->lit_reg.reg, reg->contents);
					printf("\t Register: %s\n", inst->lit_reg.reg);

					strcpy(inst->lit_reg.lit, lit->children[1]->contents);
					printf("\t Literal: $%s\n", inst->lit_reg.lit);

				}
				else if (strstr(instruction_type->tag, "reg_mem")) {
					mpc_ast_t *reg = instruction_type->children[0];
					mpc_ast_t *mem = instruction_type->children[2];

					inst->type = REG_MEM;
					inst->type_size = 4;

					strcpy(inst->reg_mem.reg, reg->contents);
					printf("\t Register: %s\n", inst->reg_mem.reg);

					strcpy(inst->reg_mem.mem, mem->children[1]->contents);
					printf("\t Memory address: &%s\n", inst->reg_mem.mem);
				}
				else if (strstr(instruction_type->tag, "mem_reg")) {
					mpc_ast_t *mem = instruction_type->children[0];
					mpc_ast_t *reg = instruction_type->children[2];

					inst->type = MEM_REG;
					inst->type_size = 4;

					strcpy(inst->reg_mem.mem, mem->children[1]->contents);
					printf("\t Memory address: &%s\n", inst->reg_mem.mem);

					strcpy(inst->reg_mem.reg, reg->contents);
					printf("\t Register: %s\n", inst->reg_mem.reg);
				}
				else if (strstr(instruction_type->tag, "lit_mem")) {
					mpc_ast_t *lit = instruction_type->children[0];	
					mpc_ast_t *mem = instruction_type->children[2];

					inst->type = LIT_MEM;
					inst->type_size = 5;

					strcpy(inst->lit_mem.lit, lit->children[1]->contents);
					printf("\t Literal: $%s\n", inst->lit_mem.lit);

					strcpy(inst->lit_mem.mem, mem->children[1]->contents);
					printf("\t Memory address: &%s\n", inst->lit_mem.mem);
				}
				else if (strstr(instruction_type->tag, "reg_ptr_reg")) {
					mpc_ast_t *reg1 = instruction_type->children[1];	
					mpc_ast_t *reg2 = instruction_type->children[3];

					inst->type = REG_PTR_REG;
					inst->type_size = 3;

					strcpy(inst->reg_reg.reg1, reg1->contents);
					printf("\t Register 1: &%s\n", inst->reg_reg.reg1);

					strcpy(inst->reg_reg.reg2, reg2->contents);
					printf("\t Register 2: %s\n", inst->reg_reg.reg2);
				}
				else if (strstr(instruction_type->tag, "lit_off_reg")) {
					mpc_ast_t *lit = instruction_type->children[0];
					mpc_ast_t *reg1 = instruction_type->children[3];
					mpc_ast_t *reg2 = instruction_type->children[5];

					inst->type = LIT_OFF_REG;
					inst->type_size = 5;

					strcpy(inst->lit_off_reg.lit, lit->children[1]->contents);
					printf("\t Literal: $%s\n", inst->lit_off_reg.lit);

					strcpy(inst->lit_off_reg.reg1, reg1->contents);
					printf("\t Register 1: &%s\n", inst->lit_off_reg.reg1);

					strcpy(inst->lit_off_reg.reg2, reg2->contents);
					printf("\t Register 2: %s\n", inst->lit_off_reg.reg2);
				}
				else {
					printf("\t Instruction type not supported yet\n");
				}
				printf("Instruction Size: %d\n", inst->type_size);
			}
		}
		printf("------------------------\n");
	}
	else {
		mpc_err_print(r.error);
		mpc_err_delete(r.error);
	}

	return inst;
}

int generate_opcode(parser_instruction_t *inst) {
	if (strstr(inst->mnemonic, "mov")) {	
		if (inst->type == LIT_REG) return 0x10;
		else if (inst->type == REG_REG) return 0x11;
		else if (inst->type == REG_MEM) return 0x12;
		else if (inst->type == MEM_REG) return 0x13;
		else if (inst->type == LIT_MEM) return 0x1b;
		else if (inst->type == REG_PTR_REG) return 0x1c;
		else if (inst->type == LIT_OFF_REG) return 0x1d;
	}

	else if (strstr(inst->mnemonic, "add")) {
		if (inst->type == REG_REG) return 0x14;
		else if (inst->type == LIT_REG) return 0x3f;
	}
	else if (strstr(inst->mnemonic, "sub")) {
		if (inst->type == LIT_REG) return 0x16;
		else if (inst->type == REG_LIT) return 0x1e;
		else if (inst->type == REG_REG) return 0x1f;
	}
	else if (strstr(inst->mnemonic, "inc")) {
		return 0x35;
	}
	else if (strstr(inst->mnemonic, "dec")) {
		return 0x36;
	}
	else if (strstr(inst->mnemonic, "mul")) {
		if (inst->type == LIT_REG) return 0x20;
		else if (inst->type == REG_REG) return 0x21;
	}

	else if (strstr(inst->mnemonic, "lsf")) {
		if (inst->type == REG_LIT | inst->type == REG_LIT8) return 0x26;
		else if (inst->type == REG_REG) return 0x27;
	}
	else if (strstr(inst->mnemonic, "rsf")) {
		if (inst->type == REG_LIT | inst->type == REG_LIT8) return 0x2a;
		else if (inst->type == REG_REG) return 0x2b;
	}
	else if (strstr(inst->mnemonic, "and")) {
		if (inst->type == REG_LIT | inst->type == REG_LIT8) return 0x2e;
		else if (inst->type == REG_REG) return 0x2f;
	}
	else if (strstr(inst->mnemonic, "or")) {
		if (inst->type == REG_LIT | inst->type == REG_LIT8) return 0x30;
		else if (inst->type == REG_REG) return 0x31;
	}
	else if (strstr(inst->mnemonic, "xor")) {
		if (inst->type == REG_LIT | inst->type == REG_LIT8) return 0x32;
		else if (inst->type == REG_REG) return 0x33;
	}
	else if (strstr(inst->mnemonic, "not")) {
		return 0x34;
	}

	else if (strstr(inst->mnemonic, "jne")) {
		if (inst->type == LIT_MEM) return 0x15;
		else if (inst->type == REG_MEM) return 0x40;
	}
	else if (strstr(inst->mnemonic, "jeq")) {
		if (inst->type == REG_MEM) return 0x3e;
		else if (inst->type == LIT_MEM) return 0x41;
	}
	else if (strstr(inst->mnemonic, "jlt")) {
		if (inst->type == LIT_MEM) return 0x42;
		else if (inst->type == REG_MEM) return 0x43;
	}
	else if (strstr(inst->mnemonic, "jgt")) {
		if (inst->type == LIT_MEM) return 0x44;
		else if (inst->type == REG_MEM) return 0x45;
	}
	else if (strstr(inst->mnemonic, "jle")) {
		if (inst->type == LIT_MEM) return 0x46;
		else if (inst->type == REG_MEM) return 0x47;
	}
	else if (strstr(inst->mnemonic, "jge")) {
		if (inst->type == LIT_MEM) return 0x48;
		else if (inst->type == REG_MEM) return 0x49;
	}

	else if (strstr(inst->mnemonic, "psh")) {
		if (inst->type == SINGLE_LIT) return 0x17;
		else if (inst->type == SINGLE_REG) return 0x18;
	}
	else if (strstr(inst->mnemonic, "pop")) {
		return 0x1a;
	}

	else if (strstr(inst->mnemonic, "cal")) {
		if (inst->type == SINGLE_LIT) return 0x5e;
		else if (inst->type == SINGLE_REG) return 0x5f;
	}
	else if (strstr(inst->mnemonic, "ret")) {
		return 0x60;
	}

	else if (strstr(inst->mnemonic, "hlt")) {
		return 0xff;
	}
	
	return -1;
}

int map_reg_to_num(const char *str) {
	// Use a switch statement to map strings to unique numbers
	if (strcmp(str, "ip") == 0)
		return 0;
	else if (strcmp(str, "acc") == 0)
		return 1;
	else if (strcmp(str, "r1") == 0)
		return 2;
	else if (strcmp(str, "r2") == 0)
		return 3;
	else if (strcmp(str, "r3") == 0)
		return 4;
	else if (strcmp(str, "r4") == 0)
		return 5;
	else if (strcmp(str, "r5") == 0)
		return 6;
	else if (strcmp(str, "r6") == 0)
		return 7;
	else if (strcmp(str, "r7") == 0)
		return 8;
	else if (strcmp(str, "r8") == 0)
		return 9;
	else if (strcmp(str, "sp") == 0)
		return 10;
	else if (strcmp(str, "fp") == 0)
		return 11;
    // Return a default value (you can choose any value you like)
    return -1;
}

void generate_machine_code(parser_instruction_t *inst, int *machine_code) {
	int i = 0;
	int opcode = generate_opcode(inst);	
	machine_code[i++] = opcode;	

	switch (inst->type) {

        case LIT_REG: {
			int hex_val = (int)strtol(inst->lit_reg.lit, NULL, 16);
			int reg = map_reg_to_num(inst->lit_reg.reg);
			machine_code[i++] = (hex_val & 0xff00) >> 8;
			machine_code[i++] = (hex_val & 0x00ff);
			machine_code[i++] = reg;
		} break;
        case REG_LIT: {
			int reg = map_reg_to_num(inst->lit_reg.reg);
			int hex_val = (int)strtol(inst->lit_reg.lit, NULL, 16);
			machine_code[i++] = reg;
			machine_code[i++] = (hex_val & 0xff00) >> 8;
			machine_code[i++] = (hex_val & 0x00ff);
		} break;
        case REG_LIT8: {
			int reg = map_reg_to_num(inst->lit_reg.reg);
			int hex_val = (int)strtol(inst->lit_reg.lit, NULL, 16);	
			machine_code[i++] = reg;
			machine_code[i++] = hex_val;
		} break;
        case REG_REG: {
			int reg1 = map_reg_to_num(inst->reg_reg.reg1);
			int reg2 = map_reg_to_num(inst->reg_reg.reg2);
			machine_code[i++] = reg1;	
			machine_code[i++] = reg2;	
		} break;
        case REG_MEM: {
			int reg = map_reg_to_num(inst->reg_mem.reg);
			int hex_val = (int)strtol(inst->reg_mem.mem, NULL, 16);
			machine_code[i++] = reg;
			machine_code[i++] = (hex_val & 0xff00) >> 8;
			machine_code[i++] = (hex_val & 0x00ff);
		} break;
        case MEM_REG: {
			int hex_val = (int)strtol(inst->reg_mem.mem, NULL, 16);
			int reg = map_reg_to_num(inst->reg_mem.reg);
			machine_code[i++] = (hex_val & 0xff00) >> 8;
			machine_code[i++] = (hex_val & 0x00ff);
			machine_code[i++] = reg;
		} break;
        case LIT_MEM: {
			int lit = (int)strtol(inst->lit_mem.lit, NULL, 16);	
			int mem = (int)strtol(inst->lit_mem.mem, NULL, 16);	
			machine_code[i++] = (lit & 0xff00) >> 8;
			machine_code[i++] = (lit & 0x00ff);
			machine_code[i++] = (mem & 0xff00) >> 8;
			machine_code[i++] = (mem & 0x00ff);
		} break;
        case REG_PTR_REG: {
			int reg1 = map_reg_to_num(inst->reg_reg.reg1);
			int reg2 = map_reg_to_num(inst->reg_reg.reg2);
			machine_code[i++] = reg1;	
			machine_code[i++] = reg2;	
		} break;
        case LIT_OFF_REG: {
			int lit = (int)strtol(inst->lit_off_reg.lit, NULL, 16);
			int reg1 = map_reg_to_num(inst->lit_off_reg.reg1);
			int reg2 = map_reg_to_num(inst->lit_off_reg.reg2);
			machine_code[i++] = (lit & 0xff00) >> 8;
			machine_code[i++] = (lit & 0x00ff);
			machine_code[i++] = reg1;	
			machine_code[i++] = reg2;	
		} break;
        case NO_ARGS: break;
        case SINGLE_REG: 
			machine_code[i++] = map_reg_to_num(inst->single_reg);	
			break;
        case SINGLE_LIT: {
			int lit = (int)strtol(inst->single_lit, NULL, 16);
			machine_code[i++] = (lit & 0xff00) >> 8;
			machine_code[i++] = (lit & 0x00ff);	
		} break;
	}
}


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
		line_count++;
	}

	fclose(file);

	if (line_count == 0) {
		printf("The file is empty.\n");
	} else {
		for (size_t i = 0; i < line_count; i++) {
			if (strcmp(lines[i], "") == 0) continue;
			printf("Line %zu: %s\n", i + 1, lines[i]);
			parser_instruction_t *inst = run(parser, "input", lines[i]);

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
	}

	free(lines); // Free the array itself
	return 0;
}

int main(int argc, char **argv) {

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
	mpc_parser_t *jeq = mpc_new("jeq");
	mpc_parser_t *jne = mpc_new("jne");
	mpc_parser_t *jlt = mpc_new("jlt");
	mpc_parser_t *jgt = mpc_new("jgt");
	mpc_parser_t *jle = mpc_new("jle");
	mpc_parser_t *jge = mpc_new("jge");
	mpc_parser_t *psh = mpc_new("psh");
	mpc_parser_t *pop = mpc_new("pop");
	mpc_parser_t *cal = mpc_new("cal");
	mpc_parser_t *ret = mpc_new("ret");
	mpc_parser_t *hlt = mpc_new("hlt");

	mpc_parser_t *lit = mpc_new("lit");
	mpc_parser_t *lit_reg = mpc_new("lit_reg");
	mpc_parser_t *reg_lit = mpc_new("reg_lit");
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
			jeq,
			jne,
			jlt,
			jgt,
			jle,
			jge,
			psh,
			pop,
			cal,
			ret,
			hlt,

			lit,
			lit_reg,
			reg_lit,
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

	if (argc < 2) {
		printf("Missing argument: %s <asm_file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	else {
		run_file(assembler, argv[1]);
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
	mpc_delete(jeq);
	mpc_delete(jne);
	mpc_delete(jlt);
	mpc_delete(jgt);
	mpc_delete(jle);
	mpc_delete(jge);
	mpc_delete(psh);
	mpc_delete(pop);
	mpc_delete(cal);
	mpc_delete(ret);
	mpc_delete(hlt);

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
