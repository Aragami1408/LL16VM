#include "expr_parser.h"

expr_parser_t *expr_parser_init(void) {
	expr_parser_t *expr_parser = (expr_parser_t *) malloc(sizeof(expr_parser_t));

	expr_parser->square_bracket_expr = mpc_new("square_bracket_expr");
	expr_parser->expr = mpc_new("expr");
	expr_parser->term = mpc_new("term");
	expr_parser->factor = mpc_new("factor");

	return expr_parser;
}

void expr_parser_free(expr_parser_t *expr_parser) {
	mpc_delete(expr_parser->square_bracket_expr);
	mpc_delete(expr_parser->expr);
	mpc_delete(expr_parser->term);
	mpc_delete(expr_parser->factor);

	free(expr_parser);	
}
