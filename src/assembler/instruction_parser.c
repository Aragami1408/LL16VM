#include "instruction_parser.h"

opcode_parsers_t *opcode_parsers_init(void) {
	opcode_parsers_t *opc = (opcode_parsers_t *) malloc(sizeof(opcode_parsers_t));

	opc->mov = mpc_new("mov");
	opc->add = mpc_new("add");
	opc->sub = mpc_new("sub");
	opc->mul = mpc_new("mul");
	opc->lsf = mpc_new("lsf");
	opc->rsf = mpc_new("rsf");
	opc->and = mpc_new("and");
	opc->or = mpc_new("or");
	opc->xor = mpc_new("xor");
	opc->inc = mpc_new("inc");
	opc->dec = mpc_new("dec");
	opc->not = mpc_new("not");
	opc->jeq = mpc_new("jeq");
	opc->jne = mpc_new("jne");
	opc->jlt = mpc_new("jlt");
	opc->jgt = mpc_new("jgt");
	opc->jle = mpc_new("jle");
	opc->jge = mpc_new("jge");
	opc->psh = mpc_new("psh");
	opc->pop = mpc_new("pop");
	opc->cal = mpc_new("cal");
	opc->ret = mpc_new("ret");
	opc->hlt = mpc_new("hlt");

	return opc;
}

void opcode_parsers_free(opcode_parsers_t *opc) {
	mpc_delete(opc->mov);
	mpc_delete(opc->add);
	mpc_delete(opc->sub);
	mpc_delete(opc->mul);
	mpc_delete(opc->lsf);
	mpc_delete(opc->rsf);
	mpc_delete(opc->and);
	mpc_delete(opc->or);
	mpc_delete(opc->xor);
	mpc_delete(opc->inc);
	mpc_delete(opc->dec);
	mpc_delete(opc->not);
	mpc_delete(opc->jeq);
	mpc_delete(opc->jne);
	mpc_delete(opc->jlt);
	mpc_delete(opc->jgt);
	mpc_delete(opc->jle);
	mpc_delete(opc->jge);
	mpc_delete(opc->psh);
	mpc_delete(opc->pop);
	mpc_delete(opc->cal);
	mpc_delete(opc->ret);
	mpc_delete(opc->hlt);

	free(opc);
}

inst_type_parsers_t *inst_type_parsers_init(void) {
	inst_type_parsers_t *inst_type = (inst_type_parsers_t *) malloc(sizeof(inst_type_parsers_t));

	inst_type->lit = mpc_new("lit");
	inst_type->lit_reg = mpc_new("lit_reg");
	inst_type->reg_lit = mpc_new("reg_lit");
	inst_type->reg_reg = mpc_new("reg_reg");
	inst_type->reg_mem = mpc_new("reg_mem");
	inst_type->mem_reg = mpc_new("mem_reg");
	inst_type->lit_mem = mpc_new("lit_mem");
	inst_type->reg_ptr_reg = mpc_new("reg_ptr_reg");
	inst_type->lit_off_reg = mpc_new("lit_off_reg");

	return inst_type;
}

void inst_type_parsers_free(inst_type_parsers_t *inst_type) {
	mpc_delete(inst_type->lit);
	mpc_delete(inst_type->lit_reg);
	mpc_delete(inst_type->reg_lit);
	mpc_delete(inst_type->reg_reg);
	mpc_delete(inst_type->reg_mem);
	mpc_delete(inst_type->mem_reg);
	mpc_delete(inst_type->lit_mem);
	mpc_delete(inst_type->reg_ptr_reg);
	mpc_delete(inst_type->lit_off_reg);

	free(inst_type);
}

