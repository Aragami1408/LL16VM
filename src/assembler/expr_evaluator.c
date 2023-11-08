#include "expr_evaluator.h"

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
