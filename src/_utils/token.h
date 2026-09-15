/**
 * @file token.h
 * @brief Token module interface for IFJ26 compiler.
 * @author Aleš Obr (skolp)
 * @note Standard: ISO C11
 */

#ifndef TOKEN_H
#define TOKEN_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Enumeration of token types.
 */
typedef enum token_type {
	TOKEN_EOF,

	// Identifier and keywords
	TOKEN_IDENTIFIER,
	TOKEN_KEYWORD_DEF,
	TOKEN_KEYWORD_VAR,
	TOKEN_KEYWORD_VAL,
	TOKEN_KEYWORD_IF,
	TOKEN_KEYWORD_ELSE,
	TOKEN_KEYWORD_WHILE,
	TOKEN_KEYWORD_RETURN,

	// Literals
	TOKEN_LIT_INT,
	TOKEN_LIT_FLOAT,
	TOKEN_LIT_STRING,

	// Arithmetic operators
	TOKEN_PLUS,
	TOKEN_MINUS,
	TOKEN_MUL,
	TOKEN_DIV,

	// Relational operators and assignment
	TOKEN_ASSIGN,
	TOKEN_EQ,
	TOKEN_NEQ,
	TOKEN_LT,
	TOKEN_LTE,
	TOKEN_GT,
	TOKEN_GTE,

	// Parentheses and punctuation
	TOKEN_LPAREN,
	TOKEN_RPAREN,
	TOKEN_LBRACE,
	TOKEN_RBRACE,
	TOKEN_COLON,
	TOKEN_SEMICOLON,
	TOKEN_COMMA
} TokenType;


/**
 * @brief Union to hold the value of a token.
 */
typedef union token_value {
	int64_t int_val;
	double float_val;
	char *str_val;
} TokenValue;

/**
 * @brief Structure to represent a token.
 */
typedef struct token {
	TokenType type;
	TokenValue data;
	size_t line;
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

#endif // TOKEN_H
