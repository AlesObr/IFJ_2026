/**
 * @project IFJ26 Compiler
 * @file stack.h
 * @brief Stack module interface for IFJ26 compiler.
 * @author Aleš Obr (xobrale00)
 * @note Standard: ISO C11
 */

#ifndef IFJ26_STACK_H
#define IFJ26_STACK_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Represents an element in the stack.
 */
typedef struct Stack_Element {
	void *data;					/**< Pointer to the data of the stack element */
	struct Stack_Element *next; /**< Pointer to the next element in the stack */
} StackElement;

/**
 * @brief Represents a stack data structure.
 */

typedef struct Stack {
	struct Stack_Element *data; /**< Pointer to the stack data array */
	struct Stack_Element *top;	/**< Pointer to the top element of the stack */
	size_t size;				/**< Current number of elements in the stack */
} Stack;

/**
 * @brief Initializes a stack.
 * @param stack Pointer to the stack to be initialized.
 */
void stack_init(Stack *stack);

/**
 * @brief Frees the memory allocated for the stack.
 * @param stack Pointer to the stack to be freed.
 */
void stack_free(Stack *stack);

/**
 * @brief Pushes an element onto the stack.
 * @param stack Pointer to the stack.
 * @param element Pointer to the element to be pushed.
 */
void stack_push(Stack *stack, void *element);

/**
 * @brief Pops an element from the stack.
 * @param stack Pointer to the stack.
 * @return Pointer to the popped element, or NULL if the stack is empty.
 */
StackElement *stack_pop(Stack *stack);

/**
 * @brief Returns the top element of the stack without removing it.
 * @param stack Pointer to the stack.
 * @return Pointer to the top element, or NULL if the stack is empty.
 */
StackElement *stack_top(Stack *stack);

#endif /* IFJ26_STACK_H */
