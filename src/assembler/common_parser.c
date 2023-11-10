#include "common_parser.h"

common_parsers_t *common_parsers_init(void) {
	common_parsers_t *common_parsers = (common_parsers_t *) malloc(sizeof(common_parsers_t));

	common_parsers->hex_literal = mpc_new("hex_literal");
	common_parsers->address = mpc_new("address");
	common_parsers->registers = mpc_new("registers");
	common_parsers->gp_registers = mpc_new("gp_registers");
	common_parsers->variable = mpc_new("variable");
	common_parsers->label = mpc_new("label");

	return common_parsers;
}

void common_parsers_free(common_parsers_t *common_parsers) {
	
	mpc_delete(common_parsers->hex_literal);
	mpc_delete(common_parsers->address);
	mpc_delete(common_parsers->registers);
	mpc_delete(common_parsers->gp_registers);
	mpc_delete(common_parsers->variable);
	mpc_delete(common_parsers->label);

	free(common_parsers);
}
