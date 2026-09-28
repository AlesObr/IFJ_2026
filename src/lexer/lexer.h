/**
 * @project IFJ26 Compiler
 * @file lexer.h
 * @brief Lexer module interface for IFJ26 compiler.
 * @author Aleš Obr (xobrale00)
 * @author Jiří Kolek (xkolekj00)
 * @author admiral <Jkolikovec@gmail.com>
 * @note Standard: ISO C11
 */

#ifndef IFJ26_LEXER_H
#define IFJ26_LEXER_H

#include "../_utils/error.h"
#include "../_utils/token.h"
#include <stdio.h>

/**
 * @brief Enumeration of lexer states.
 */
typedef enum {
	STATE_START,
	STATE_IDENTIFIER,
	STATE_WANNABE_FLOAT,
	STATE_WANNABE_INT,
	STATE_NUMBER,
	STATE_LEADING_ZERO,
	STATE_FLOAT, /**< State for double */
	STATE_STRING,
	STATE_COMMENT,
	STATE_ML_COMMENT,
	STATE_OPERATOR,
	STATE_ERROR
} LexerState;

/**
 * @brief Support structure for lexemes, keeps track of previous position for pushbacks
 */
typedef struct {
	size_t previous_line;
	size_t previous_col;
	int current_lexeme;
} LexemeState;

/**
 * @brief Initializes the lexer with an input stream.
 * @param input Open file stream to read source characters from (e.g. stdin).
 */
void lexer_init(FILE *input);

/**
 * @brief Reads characters from the input stream and returns the next token.
 *
 * Functions on a pull principle. Advances input until a full token is recognized.
 * If a lexical error occurs, the HErBA protocol will be involved.
 *
 * @param out_token Pointer to the token structure to be filled with the next token. If error
 * occurs, the token will be in a valid, destructible state.
 * @return Error code indicating success or failure of the operation.
 */
Error lexer_get_next_token(Token *out_token);

/**
 * @brief Loads in a single character from the input stream.
 *
 * @param lexeme The character to be read and companion information for correct pushbacks
 * @param input Opened stream to be read from
 */
int lexer_take(LexemeState *lexeme, FILE *input);

/**
 * @brief Return a single character to the input stream.
 *
 * @param lexeme The character to be returned and companion information for correct pushbacks
 * @param input Opened stream where the character will be returned to
 */
void lexer_give(LexemeState *lexeme, FILE *input);

#endif /* IFJ26_LEXER_H */
