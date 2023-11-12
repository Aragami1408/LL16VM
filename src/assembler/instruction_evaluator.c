#include "instruction_evaluator.h"

int __generate_opcode(parser_instruction_t *inst) {
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

int __map_reg_to_num(const char *str) {
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

parser_instruction_t *evaluate_instruction(mpc_parser_t *parser, const char *filename, const char *input, const label_hashmap_t *label_map) {

	mpc_result_t r;
	parser_instruction_t *inst = (parser_instruction_t *) malloc(sizeof(parser_instruction_t));	

	if(mpc_parse(filename, input, parser, &r)) {
		mpc_ast_t *ast = r.output;
		if (strstr(input, "ret") || strstr(input, "hlt")) {
			strcpy(inst->mnemonic, ast->tag);
			inst->type = NO_ARGS;
			inst->type_size = 1;
		}
		else {
			mpc_ast_t *mnemonic = ast->children[0];
			if (strstr(mnemonic->tag, "label")) {
				return NULL;
			}
			strcpy(inst->mnemonic, mnemonic->tag);


			mpc_ast_t *instruction_type = mnemonic->children[1];
			if (strstr(instruction_type->tag, "registers")) {
				mpc_ast_t *reg = instruction_type;
				
				// strcpy(inst->type, "single_reg|>");
				inst->type = SINGLE_REG;

				inst->type_size = 2;

				strcpy(inst->single_reg, reg->contents);
			}
			else if (strstr(instruction_type->tag, "lit|hex_literal")) {
				mpc_ast_t *lit = instruction_type;

				// strcpy(inst->type, "single_lit|>");
				inst->type = SINGLE_LIT;

				inst->type_size = 3;
				
				strcpy(inst->single_lit, lit->children[1]->contents);
			}
			else if (strstr(instruction_type->tag, "square_bracket_expr")) {
				mpc_ast_t *lit = instruction_type;

				// strcpy(inst->type, "single_lit|>");
				inst->type = SINGLE_LIT;

				inst->type_size = 3;

				int result = evaluate_expression(lit, label_map);	
				snprintf(inst->single_lit, 5, "%04x", result); 
			}
			else {
				
				// strcpy(inst->type, instruction_type->tag);
				if (strstr(instruction_type->tag, "lit_reg")) {
					mpc_ast_t *lit = instruction_type->children[0];	
					mpc_ast_t *reg = instruction_type->children[2];

					inst->type = LIT_REG;
					inst->type_size = 4;
					
					if (strstr(lit->tag, "hex_literal")) {	
						strcpy(inst->lit_reg.lit, lit->children[1]->contents);
					}
					else if (strstr(lit->tag, "square_bracket_expr")) {
						int result = evaluate_expression(lit, label_map);
						snprintf(inst->lit_reg.lit, 5, "%04x", result);
					}

					strcpy(inst->lit_reg.reg, reg->contents);
				}
				else if(strstr(instruction_type->tag, "reg_reg")) {
					mpc_ast_t *reg1 = instruction_type->children[0];	
					mpc_ast_t *reg2 = instruction_type->children[2];

					inst->type = REG_REG;
					inst->type_size = 3;

					strcpy(inst->reg_reg.reg1, reg1->contents);

					strcpy(inst->reg_reg.reg2, reg2->contents);
				
				}
				else if (strstr(instruction_type->tag, "reg_lit")) {
					mpc_ast_t *reg = instruction_type->children[0];	
					mpc_ast_t *lit = instruction_type->children[2];
					
					if (strstr(inst->mnemonic, "lsf") || strstr(inst->mnemonic, "rsf")) {
						inst->type = REG_LIT8;
						inst->type_size = 3;
					} else {
						inst->type = REG_LIT;
						inst->type_size = 4;
					}

					strcpy(inst->lit_reg.reg, reg->contents);

					if (strstr(lit->tag, "hex_literal")) {
						strcpy(inst->lit_reg.lit, lit->children[1]->contents);
					}
					else if (strstr(lit->tag, "square_bracket_expr")) {
						int result = evaluate_expression(lit, label_map);
						snprintf(inst->lit_reg.lit, 5, "%04x", result);
					}


				}
				else if (strstr(instruction_type->tag, "reg_mem")) {
					mpc_ast_t *reg = instruction_type->children[0];

					inst->type = REG_MEM;
					inst->type_size = 4;

					strcpy(inst->reg_mem.reg, reg->contents);

					if (instruction_type->children_num < 4) {
						mpc_ast_t *address = instruction_type->children[2];
						strcpy(inst->reg_mem.mem, address->children[1]->contents);
					}
					else {
						mpc_ast_t *expr = instruction_type->children[3];
						int result = evaluate_expression(expr, label_map);
						snprintf(inst->reg_mem.mem, 5, "%04x", result);
					}

				}
				else if (strstr(instruction_type->tag, "mem_reg")) {

					inst->type = MEM_REG;
					inst->type_size = 4;

					if (instruction_type->children_num < 4) {
						mpc_ast_t *mem = instruction_type->children[0];
						strcpy(inst->reg_mem.mem, mem->children[1]->contents);

						mpc_ast_t *reg = instruction_type->children[2];
						strcpy(inst->reg_mem.reg, reg->contents);
					}
					else {
						mpc_ast_t *expr = instruction_type->children[1];
						int result = evaluate_expression(expr, label_map);
						snprintf(inst->reg_mem.mem, 5, "%04x", result);

						mpc_ast_t *reg = instruction_type->children[3];
						strcpy(inst->reg_mem.reg, reg->contents);
					}

				}
				else if (strstr(instruction_type->tag, "lit_mem")) {
					mpc_ast_t *lit = instruction_type->children[0];	

					inst->type = LIT_MEM;
					inst->type_size = 5;
					
					if (strstr(lit->tag, "hex_literal")) {	
						strcpy(inst->lit_mem.lit, lit->children[1]->contents);
					}
					else if (strstr(lit->tag, "square_bracket_expr")) {
						int result = evaluate_expression(lit, label_map);
						snprintf(inst->lit_mem.lit, 5, "%04x", result);
					}

					if (instruction_type->children_num < 4) {
						mpc_ast_t *mem = instruction_type->children[2];
						strcpy(inst->lit_mem.mem, mem->children[1]->contents);
					}
					else {
						mpc_ast_t *mem = instruction_type->children[3];
						int result = evaluate_expression(mem, label_map);
						snprintf(inst->lit_mem.mem, 5, "%04x", result);
					}
				}
				else if (strstr(instruction_type->tag, "reg_ptr_reg")) {
					mpc_ast_t *reg1 = instruction_type->children[1];	
					mpc_ast_t *reg2 = instruction_type->children[3];

					inst->type = REG_PTR_REG;
					inst->type_size = 3;

					strcpy(inst->reg_reg.reg1, reg1->contents);

					strcpy(inst->reg_reg.reg2, reg2->contents);
				}
				else if (strstr(instruction_type->tag, "lit_off_reg")) {
					mpc_ast_t *lit = instruction_type->children[0];
					mpc_ast_t *reg1 = instruction_type->children[3];
					mpc_ast_t *reg2 = instruction_type->children[5];

					inst->type = LIT_OFF_REG;
					inst->type_size = 5;

					if (strstr(lit->tag, "hex_literal")) {
						strcpy(inst->lit_off_reg.lit, lit->children[1]->contents);
					}
					else if (strstr(lit->tag, "square_bracket_expr")) {
						int result = evaluate_expression(lit, label_map);
						snprintf(inst->lit_mem.lit, 5, "%04x", result);
					}

					strcpy(inst->lit_off_reg.reg1, reg1->contents);

					strcpy(inst->lit_off_reg.reg2, reg2->contents);
				}
				else {
					printf("\t Instruction type not supported yet\n");
				}
			}
		}
	}
	else {
		mpc_err_print(r.error);
		mpc_err_delete(r.error);
	}

	return inst;
}

void generate_machine_code(parser_instruction_t *inst, int *machine_code) {
	int i = 0;
	int opcode = __generate_opcode(inst);	
	machine_code[i++] = opcode;	

	switch (inst->type) {

        case LIT_REG: {
			int hex_val = (int)strtol(inst->lit_reg.lit, NULL, 16);
			int reg = __map_reg_to_num(inst->lit_reg.reg);
			machine_code[i++] = (hex_val & 0xff00) >> 8;
			machine_code[i++] = (hex_val & 0x00ff);
			machine_code[i++] = reg;
		} break;
        case REG_LIT: {
			int reg = __map_reg_to_num(inst->lit_reg.reg);
			int hex_val = (int)strtol(inst->lit_reg.lit, NULL, 16);
			machine_code[i++] = reg;
			machine_code[i++] = (hex_val & 0xff00) >> 8;
			machine_code[i++] = (hex_val & 0x00ff);
		} break;
        case REG_LIT8: {
			int reg = __map_reg_to_num(inst->lit_reg.reg);
			int hex_val = (int)strtol(inst->lit_reg.lit, NULL, 16);	
			machine_code[i++] = reg;
			machine_code[i++] = hex_val;
		} break;
        case REG_REG: {
			int reg1 = __map_reg_to_num(inst->reg_reg.reg1);
			int reg2 = __map_reg_to_num(inst->reg_reg.reg2);
			machine_code[i++] = reg1;	
			machine_code[i++] = reg2;	
		} break;
        case REG_MEM: {
			int reg = __map_reg_to_num(inst->reg_mem.reg);
			int hex_val = (int)strtol(inst->reg_mem.mem, NULL, 16);
			machine_code[i++] = reg;
			machine_code[i++] = (hex_val & 0xff00) >> 8;
			machine_code[i++] = (hex_val & 0x00ff);
		} break;
        case MEM_REG: {
			int hex_val = (int)strtol(inst->reg_mem.mem, NULL, 16);
			int reg = __map_reg_to_num(inst->reg_mem.reg);
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
			int reg1 = __map_reg_to_num(inst->reg_reg.reg1);
			int reg2 = __map_reg_to_num(inst->reg_reg.reg2);
			machine_code[i++] = reg1;	
			machine_code[i++] = reg2;	
		} break;
        case LIT_OFF_REG: {
			int lit = (int)strtol(inst->lit_off_reg.lit, NULL, 16);
			int reg1 = __map_reg_to_num(inst->lit_off_reg.reg1);
			int reg2 = __map_reg_to_num(inst->lit_off_reg.reg2);
			machine_code[i++] = (lit & 0xff00) >> 8;
			machine_code[i++] = (lit & 0x00ff);
			machine_code[i++] = reg1;	
			machine_code[i++] = reg2;	
		} break;
        case NO_ARGS: break;
        case SINGLE_REG: 
			machine_code[i++] = __map_reg_to_num(inst->single_reg);	
			break;
        case SINGLE_LIT: {
			int lit = (int)strtol(inst->single_lit, NULL, 16);
			machine_code[i++] = (lit & 0xff00) >> 8;
			machine_code[i++] = (lit & 0x00ff);	
		} break;
	}
}
