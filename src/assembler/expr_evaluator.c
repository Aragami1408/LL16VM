#include "expr_evaluator.h"
#include "label_evaluator.h"

token_list_t* create_token_list(size_t initial_capacity) {
    token_list_t* token_list = (token_list_t*)malloc(sizeof(token_list_t));
    if (token_list) {
	token_list->tokens = (token_t*)malloc(initial_capacity * sizeof(token_t));
	if (token_list->tokens) {
	    token_list->size = 0;
	    token_list->capacity = initial_capacity;
	} else {
	    free(token_list);
	    token_list = NULL;
	}
    }
    return token_list;
}

void append_token(token_list_t *token_list, token_t token) {
    if (token_list->size == token_list->capacity) {
	// If the list is full, resize it
	token_list->capacity *= 2;
	token_list->tokens = (token_t*)realloc(token_list->tokens, token_list->capacity * sizeof(token_t));
    }
    token_list->tokens[token_list->size++] = token;
}

void free_token_list(token_list_t* token_list) {
    free(token_list->tokens);
    free(token_list);
}

operator_stack_t *create_operator_stack(size_t capacity) {
    operator_stack_t *stack = (operator_stack_t *) malloc(sizeof(operator_stack_t *));

    if (stack) {
	stack->stack = (token_t *) malloc(capacity * sizeof(token_t));
	if (stack->stack) {
	    stack->top = -1;
	    stack->capacity = capacity;
	}
	else {
	    free(stack);
	    stack = NULL;
	}
    }

    return stack;
}

bool is_operator_stack_empty(operator_stack_t *stack) {
    return stack->top == -1;
}

void push_operator(operator_stack_t *stack, token_t operator) {
    if (stack->top < (int)stack->capacity - 1) {
	stack->stack[++stack->top] = operator;
    }
}

token_t pop_operator(operator_stack_t *stack) {
    if (!is_operator_stack_empty(stack)) {
	return stack->stack[stack->top--];
    }
    else {
	token_t empty_token = {0};
	return empty_token;
    }
}

void free_operator_stack(operator_stack_t *stack) {
    free(stack->stack);
    free(stack);
}

eval_stack_t *create_eval_stack(size_t capacity) {
	eval_stack_t *stack = (eval_stack_t *)malloc(sizeof(eval_stack_t));
	if (stack) {
		stack->stack = (int *)malloc(capacity * sizeof(int));
		if (stack->stack) {
			stack->top = -1;
			stack->capacity = capacity;
		}
		else {
			free(stack);
			stack = NULL;
		}
	}
	return stack;
}

bool is_eval_stack_empty(eval_stack_t *stack) {
	return stack->top == -1;
}

void push_eval(eval_stack_t *stack, int operand) {
	if (stack->top < (int)stack->capacity - 1) {
		stack->stack[++stack->top] = operand;
	}
}

int pop_eval(eval_stack_t *stack) {
	if (!is_eval_stack_empty(stack)) {
		return stack->stack[stack->top--];
	}
	else {
		return 0;
	}
}

void free_eval_stack(eval_stack_t *stack) {
	free(stack->stack);
	free(stack);
}

int __operator_precedence(token_t operator) {
	switch (operator.op) {
		case '+':
		case '-':
			return 1;
		case '*':
			return 2;
		default:
			return 0;
	}
}

bool __is_hex(const char *str) {
	if (str == NULL || str[0] == '\0') return false;

	for (int i = 0; str[i] != '\0'; i++) {
		if (!isxdigit((unsigned char)str[i])) {
			return false;
		}
	}

	return true;
}

void dfs_traversal(mpc_ast_t *node, token_list_t *token_list, const label_hashmap_t *label_map) {
    if (node == NULL) return; // TODO: Error Handling
			      
    if (node->tag) {
		token_t token = {0};

		#define MATCH_CONTENT(str) strstr(node->contents, str)

		if (MATCH_CONTENT("+") 
				|| MATCH_CONTENT("-") 
				|| MATCH_CONTENT("*")) {
			token.type = TOKEN_OP;
			token.op = node->contents[0];
			append_token(token_list, token);
		}
		else if (MATCH_CONTENT("(")) {
			token.type = TOKEN_LPAREN;
			append_token(token_list, token);
		}
		else if (MATCH_CONTENT(")")) {
			token.type = TOKEN_RPAREN;
			append_token(token_list, token);
		}
		else if (__is_hex(node->contents)) {
			token.type = TOKEN_LITERAL;
			token.val = (int)strtol(node->contents, NULL, 16);
			append_token(token_list, token);
		}
		else if (strcmp(node->contents, "") == 0 || MATCH_CONTENT("[") || MATCH_CONTENT("]") || MATCH_CONTENT("$") || MATCH_CONTENT("!")) {
			// Skip
		}

		else {
			// Find label and replace to literal if found
			int literal;
			if (find_label(label_map, node->contents, &literal)) {
				token.type = TOKEN_LITERAL;
				token.val = literal;
				append_token(token_list, token);
			}
			else {
				printf("%s label not resolved. Exit the program\n", node->contents);
				exit(EXIT_FAILURE);
			}
		}
    }

    for (int i = 0; i < node->children_num; i++) {
		dfs_traversal(node->children[i], token_list, label_map);
    }

}

void print_tokens(token_list_t *token_list) {
	if (token_list == NULL) {
		return;
	}

	for (size_t i = 0; i < token_list->size; i++) {
		token_t token = token_list->tokens[i];
		switch (token.type) {
			case TOKEN_LITERAL:
				printf("Literal Token: %d\n", token.val);
				break;
			case TOKEN_OP:
				printf("Operator Token: %c\n", token.op);
				break;
			case TOKEN_LPAREN:
				printf("Left Parenthesis Token\n");
				break;
			case TOKEN_RPAREN:
				printf("Right Parenthesis Token\n");
				break;
        }
	}
}

token_list_t *infix_to_rpn(token_list_t *infix_tokens) {

	token_list_t *rpn_tokens = create_token_list(infix_tokens->size);
	operator_stack_t *operator_stack = create_operator_stack(infix_tokens->size);

	for (size_t i = 0; i < infix_tokens->size; i++) {
		token_t token = infix_tokens->tokens[i];

		switch (token.type) {
			case TOKEN_LITERAL:
				append_token(rpn_tokens, token);
				break;
			case TOKEN_OP: {
				token_t operator_peek = operator_stack->stack[operator_stack->top];

				while (
					(!is_operator_stack_empty(operator_stack))
					&& (operator_peek.type != TOKEN_LPAREN) 
					&& (__operator_precedence(operator_peek) >= __operator_precedence(token))) {

						append_token(rpn_tokens, pop_operator(operator_stack));
				}

				push_operator(operator_stack, token);
			} break;   
			case TOKEN_LPAREN:
				push_operator(operator_stack, token);
			break;
			case TOKEN_RPAREN: {
				token_t operator_peek = operator_stack->stack[operator_stack->top];
				while ((operator_peek.type != TOKEN_LPAREN) && !is_operator_stack_empty(operator_stack)) {
					append_token(rpn_tokens, pop_operator(operator_stack));
				}
				if (operator_stack->stack[operator_stack->top].type == TOKEN_LPAREN) 
					pop_operator(operator_stack);
			} break;
		}
	}

	while (!is_operator_stack_empty(operator_stack)) {
		token_t op = pop_operator(operator_stack);
		if (op.op != TOKEN_LPAREN)
			append_token(rpn_tokens, op);
	}

	free_operator_stack(operator_stack);

	return rpn_tokens;
}

int evaluate_postfix(token_list_t *postfix_tokens) {
	eval_stack_t *eval_stack = create_eval_stack(postfix_tokens->size);

	for (size_t i = 0; i < postfix_tokens->size; i++) {
		token_t token = postfix_tokens->tokens[i];

		switch (token.type) {
			case TOKEN_LITERAL:
				push_eval(eval_stack, token.val);
			break;
			case TOKEN_OP: {
				int operand2 = pop_eval(eval_stack);
				int operand1 = pop_eval(eval_stack);
				int result;

				switch (token.op) {
					case '+':
						result = operand1 + operand2;
					break;
					case '-':
						result = operand1 - operand2;
					break;
					case '*':
						result = operand1 * operand2;
					break;
				}

				push_eval(eval_stack, result);
			} break;
			case TOKEN_LPAREN:
			break;
			case TOKEN_RPAREN:
						   break;
		}
	}

	int final_result = pop_eval(eval_stack);

	free_eval_stack(eval_stack);

	return final_result;
}

int evaluate_expression(mpc_ast_t *ast, const label_hashmap_t *label_map) {
	token_list_t *infix_tokens = create_token_list(INITIAL_CAPACITY);
	dfs_traversal(ast, infix_tokens, label_map);
	token_list_t *rpn_tokens = infix_to_rpn(infix_tokens);
	int result = evaluate_postfix(rpn_tokens);
	
	free_token_list(rpn_tokens);
	free_token_list(infix_tokens);

	return result;
}
