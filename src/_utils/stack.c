/**
 * @project IFJ26 Compiler
 * @file stack.c
 * @brief Stack module implementation for IFJ26 compiler.
 * @author Aleš Obr (xobrale00)
 * @note Standard: ISO C11
 */

#include "stack.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>

void stack_init(Stack *stack) {
	stack->data = NULL;
	stack->top = NULL;
	stack->size = 0;
}

void stack_free(Stack *stack) {
	while (stack->top != NULL) {
		stack_pop(stack);
	}
}

void stack_push(Stack *stack, void *element) {
	StackElement *new_element = (StackElement *)malloc(sizeof(StackElement));
	if (new_element == NULL) {
		fprintf(stderr, "Memory allocation failed in stack_push\n");
		exit(EXIT_FAILURE);
	}
	new_element->data = element;
	new_element->next = stack->top;
	stack->top = new_element;
	stack->size++;
}

StackElement *stack_pop(Stack *stack) {
	if (stack->top == NULL) {
		return NULL; // Stack is empty
	}
	StackElement *top_element = stack->top;
	void *data = top_element->data;
	stack->top = top_element->next;
	free(top_element);
	stack->size--;
	return data;
}

StackElement *stack_top(Stack *stack) {
	if (stack->top == NULL) {
		return NULL; // Stack is empty
	}
	return stack->top;
}
