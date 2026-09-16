/**
 * @project IFJ26 Compiler
 * @file lexer.h
 * @brief Lexer module interface for IFJ26 compiler.
 * @author Aleš Obr (xobrale00)
 * @note Standard: ISO C11
 */

#ifndef IFJ26_LEXER_H
#define IFJ26_LEXER_H

#include <stdio.h>
#include "../_utils/token.h"

/**
 * @brief Initializes the lexer with an input stream.
 * @param input Open file stream to read source characters from (e.g. stdin).
 */
void lexer_init(FILE *input);

/**
 * @brief Reads characters from the input stream and returns the next token.
 *
 * Functions on a pull principle. Advances input until a full token is recognized.
 * If a lexical error occurs, the implementation terminates the compiler
 * with exit code 1 (lexical error) according to IFJ specifications.
 *
 * @return Recognized token with its type, value and line number.
 */
Token lexer_get_next_token(void);

#endif /* IFJ26_LEXER_H */
