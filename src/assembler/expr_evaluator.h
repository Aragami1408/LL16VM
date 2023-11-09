#ifndef EXPR_EVALUATOR_H
#define EXPR_EVALUATOR_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "mpc.h"

#define INITIAL_CAPACITY 10

typedef enum {
	TOKEN_LITERAL,
	TOKEN_OP,
	TOKEN_LPAREN,
	TOKEN_RPAREN
} token_type_t;

typedef struct {
	token_type_t type;
	int val;
	char op;
} token_t;

typedef struct token_list {
	token_t *tokens;
	size_t size;
	size_t capacity;
} token_list_t;

token_list_t *create_token_list(size_t initial_capacity);
void append_token(token_list_t *token_list, token_t token);
void free_token_list(token_list_t *token_list);

typedef struct operator_stack {
	token_t *stack;
	int top;
	size_t capacity;
} operator_stack_t;

operator_stack_t *create_operator_stack(size_t capacity);
bool is_operator_stack_empty(operator_stack_t *stack);
void push_operator(operator_stack_t *stack, token_t operator);
token_t pop_operator(operator_stack_t *stack);
void free_operator_stack(operator_stack_t *stack);

typedef struct eval_stack {
	int *stack;
	int top;
	size_t capacity;
} eval_stack_t;

eval_stack_t *create_eval_stack(size_t capacity);
bool is_eval_stack_empty(eval_stack_t *stack);
void push_eval(eval_stack_t *stack, int operand);
int pop_eval(eval_stack_t *stack);
void free_eval_stack(eval_stack_t *stack);

void dfs_traversal(mpc_ast_t *node, token_list_t *token_list);
void print_tokens(token_list_t *token_list);
token_list_t *infix_to_rpn(token_list_t *infix_tokens);
int evaluate_postfix(token_list_t *postfix_tokens);

int evaluate_expression(mpc_ast_t *node);


#endif
