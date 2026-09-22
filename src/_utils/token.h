/**
 * @project IFJ26 Compiler
 * @file token.h
 * @brief Token module interface for IFJ26 compiler.
 * @author Aleš Obr (xobrale00)
 * @note Standard: ISO C11
 *
 * @par Ownership contract
 * Functions receiving a Token pointer borrow the instance; they do NOT take ownership.
 * The caller retains full ownership and is strictly responsible for calling token_destroy()
 * when the token is no longer needed.
 *
 * If any operation fails, the Token is guaranteed to remain in a valid, destructible state.
 */

#ifndef IFJ26_TOKEN_H
#define IFJ26_TOKEN_H

#include "../_utils/dstring.h"
#include "../_utils/error.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

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
	TOKEN_KEYWORD_EXT,
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
	TOKEN_KEYWORD_FOR,
	TOKEN_KEYWORD_THEN,
	TOKEN_KEYWORD_IMPORT,
	TOKEN_KEYWORD_RETURN,

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
	TOKEN_AT,
	TOKEN_EOL,

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
	TOKEN_NOT,

	// Parentheses and punctuation
	TOKEN_LPAREN,
	TOKEN_RPAREN,
	TOKEN_LBRACE,
	TOKEN_RBRACE,
	TOKEN_COLON,
	TOKEN_LBRACKET,
	TOKEN_RBRACKET,
	TOKEN_SEMICOLON,
	TOKEN_COMMA
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

#endif /* IFJ26_TOKEN_H */
