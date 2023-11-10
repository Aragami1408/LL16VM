#ifndef EXPR_PARSER_H
#define EXPR_PARSER_H

#include "mpc.h"

typedef struct expr_parser {
	mpc_parser_t *square_bracket_expr;
	mpc_parser_t *expr;
	mpc_parser_t *term;
	mpc_parser_t *factor;
} expr_parser_t;

expr_parser_t *expr_parser_init(void);
void expr_parser_free(expr_parser_t *expr_parser);

#endif
