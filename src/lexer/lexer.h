/**
 * @project IFJ26 Compiler
 * @file lexer.h
 * @brief Lexer module interface for IFJ26 compiler.
 * @author Aleš Obr (xobrale00)
 * @author Jirka Kolek (xkolekj00)
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
} LexerState;

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
 * @brief Loads in a single charecter from the input stream.
 *
 * @param input Opened stream to be read from
 */
void lexer_take(FILE *input);

/**
 * @brief Return a single character to the input stream.
 *
 * @param input Opened stream where the charecter will be returned to
 */
void lexer_give(FILE *input);

#endif /* IFJ26_LEXER_H */
