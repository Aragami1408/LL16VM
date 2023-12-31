#ifndef COMMON_PARSERS_H
#define COMMON_PARSERS_H

#include "mpc.h"

typedef struct common_parsers {
	// Literal parsers
	mpc_parser_t *hex_literal;
	mpc_parser_t *address;

	// Register parsers
	mpc_parser_t *registers;
	mpc_parser_t *gp_registers;

	// Identifier parsers
	mpc_parser_t *variable;
	mpc_parser_t *label;
} common_parsers_t;

common_parsers_t *common_parsers_init(void);
void common_parsers_free(common_parsers_t *common_parsers);

#endif
