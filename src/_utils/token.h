/**
 * @project IFJ26 Compiler
 * @file token.h
 * @brief Token module interface for IFJ26 compiler.
 * @author Aleš Obr (xobrale00)
 * @author Admiral (xkolekj00)
 * @note Standard: ISO C11
 */

#ifndef IFJ26_TOKEN_H
#define IFJ26_TOKEN_H

#include "../_utils/dstring.h"
#include "../_utils/error.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Enumeration of token types.
 */
typedef enum token_type {
	TOKEN_EOF,

	// Identifier and keywords
	TOKEN_IDENTIFIER,
	TOKEN_KEYWORD_DEF,
	TOKEN_KEYWORD_DO,
	TOKEN_KEYWORD_CLASS,
	TOKEN_KEYWORD_CASE,
	TOKEN_KEYWORD_EXTENSION,
	TOKEN_KEYWORD_TYPE,
	TOKEN_KEYWORD_MATCH,
	TOKEN_KEYWORD_BOOLEAN,
	TOKEN_KEYWORD_LIST,
	TOKEN_KEYWORD_FOR,
	TOKEN_KEYWORD_INT,
	TOKEN_KEYWORD_DOUBLE,
	TOKEN_KEYWORD_STRING,
	TOKEN_KEYWORD_UNIT,
	TOKEN_KEYWORD_VAR,
	TOKEN_KEYWORD_VAL,
	TOKEN_KEYWORD_IF,
	TOKEN_KEYWORD_ELSE,
	TOKEN_KEYWORD_WHILE,
	TOKEN_KEYWORD_THEN,
	TOKEN_KEYWORD_IMPORT,
	TOKEN_KEYWORD_RETURN,
	TOKEN_KEYWORD_NULL,
	TOKEN_KEYWORD_TRUE,
	TOKEN_KEYWORD_FALSE,

	// Literals
	TOKEN_LIT_INT,
	TOKEN_LIT_FLOAT, /**< In fact double */
	TOKEN_LIT_STRING,

	// Arithmetic operators
	TOKEN_PLUS,
	TOKEN_MINUS,
	TOKEN_MUL,
	TOKEN_DIV,
	TOKEN_DOT,
	TOKEN_AT_MAIN,
	TOKEN_EOL,
	TOKEN_INDENT,
	TOKEN_OUTDENT,
	TOKEN_UNDERSCORE,
	TOKEN_COLON,

	// Relational operators and assignment
	TOKEN_ASSIGN,
	TOKEN_EQ,
	TOKEN_NEQ,
	TOKEN_LT,
	TOKEN_LTE,
	TOKEN_GT,
	TOKEN_GTE,
	TOKEN_AND,
	TOKEN_OR,
	TOKEN_PIPE,
	TOKEN_NOT,

	// Parentheses and punctuation
	TOKEN_LPAREN,
	TOKEN_RPAREN,
	TOKEN_LBRACE,
	TOKEN_RBRACE,
	TOKEN_LBRACKET,
	TOKEN_RBRACKET,
	TOKEN_SEMICOLON,
	TOKEN_COMMA,
} TokenType;

/**
 * @brief Union to hold the value of a token.
 */
typedef union token_value {
	int64_t int_val;
	double float_val;
	dstring_t *str_val;
} TokenValue;

/**
 * @brief Structure to represent a token.
 */
typedef struct token {
	TokenType type;	 /** The type of the token */
	TokenValue data; /** The value of the token (if applicable) */
	size_t line;	 /** The line number where the token was found */
	size_t col;		 /** The column number where the token starts */
} Token;

/**
 * @brief Initializes a token structure to default empty state.
 * @param t Pointer to the token to initialize.
 */
void token_init(Token *t);

/**
 * @brief Frees dynamically allocated resources held by the token (e.g. str_val).
 * @param t Pointer to the token to clean up.
 */
void token_destroy(Token *t);

#include <stdbool.h>

/**
 * Helper for simple tokens without attributes (operators, punctuation, EOF, keywords).
 */
static inline Error token_make_simple(Token *t, TokenType type, size_t line, size_t col) {
	t->type = type;
	t->line = line;
	t->col = col;
	t->data.str_val = NULL;
	return ERROR_OK();
}

/**
 * Helper for integer literals.
 */
static inline Error token_make_int(Token *t, int64_t val, size_t line, size_t col) {
	t->type = TOKEN_LIT_INT;
	t->line = line;
	t->col = col;
	t->data.int_val = val;
	return ERROR_OK();
}

/**
 * Helper for floating point literals.
 */
static inline Error token_make_float(Token *t, double val, size_t line, size_t col) {
	t->type = TOKEN_LIT_FLOAT;
	t->line = line;
	t->col = col;
	t->data.float_val = val;
	return ERROR_OK();
}

/**
 * Helper for tokens with dynamic string data (identifiers, string literals).
 * Takes ownership of the allocated string buffer or copies it.
 */
/**
 * @brief Helper constructor for tokens with dynamic string data.
 * @param t Target token to initialize.
 * @param type Token type (identifier, string literal, etc.).
 * @param src Dynamic string source buffer.
 * @param line Line position in source code.
 * @param col Column position in source code.
 * @return ERROR_OK on success, ERR_INTERNAL on allocation failure.
 */
static inline Error token_make_str(
	Token *t, TokenType type, dstring_t *src, size_t line, size_t col) {
	t->type = type;
	t->line = line;
	t->col = col;

	t->data.str_val = malloc(sizeof(dstring_t));
	if (!t->data.str_val) {
		return ERROR_EMIT(ERR_INTERNAL, line, col, "Allocation failure for token string.");
	}

	if (!dstring_init(t->data.str_val) || !dstring_copy(t->data.str_val, src)) {
		free(t->data.str_val);
		t->data.str_val = NULL;
		return ERROR_EMIT(ERR_INTERNAL, line, col, "Failed to copy dynamic string.");
	}

	return ERROR_OK();
}

#endif /* IFJ26_TOKEN_H */
