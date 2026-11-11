/**
 * @project IFJ26 Compiler
 * @file error.h
 * @brief Error codes and diagnostic reporter interface.
 * @author Aleš Obr (xobrale00)
 * @note Standard: ISO C11
 */

#ifndef IFJ26_ERROR_H
#define IFJ26_ERROR_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Error codes as defined by the IFJ26 language specification.
 *
 * Numerical values correspond directly to the return codes required by the project.
 */
typedef enum {
	ERR_OK = 0,
	ERR_LEXICAL = 1,
	ERR_SYNTAX = 2,
	ERR_SEM_UNDEF = 3,
	ERR_SEM_TYPE = 4,
	ERR_INTERNAL = 99
} ErrorType;

/**
 * @brief Representation of an error payload propagated up the call stack.
 */
typedef struct {
	ErrorType type;		 /**Specific error type and process exit code. */
	size_t line;		 /** Source code line where the error occurred. */
	size_t col;			 /** Source code column where the error occurred. */
	const char *message; /** Descriptive error diagnostics message. */
} Error;

/**
 * @brief Constructs a success state indicating no error occurred.
 */
#define ERROR_OK() ((Error){.type = ERR_OK, .line = 0, .col = 0, .message = NULL})

/**
 * @brief Constructs an error descriptor with context.
 */
#define ERROR_EMIT(t, l, c, msg) ((Error){.type = (t), .line = (l), .col = (c), .message = (msg)})

/**
 * @brief Verifies whether an error descriptor represents a failure state.
 */
#define ERROR_IS_ERR(err) ((err).type != ERR_OK)

#endif /* IFJ26_ERROR_H */
