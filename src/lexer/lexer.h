/**
 * @project IFJ26 Compiler
 * @file lexer.h
 * @brief Lexer module interface for IFJ26 compiler.
 * @author Aleš Obr (xobrale00)
 * @note Standard: ISO C11
 */

#ifndef IFJ26_LEXER_H
#define IFJ26_LEXER_H

#include "../_utils/error.h"
#include "../_utils/token.h"
#include <stdio.h>

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
 * @param out_token Pointer to the token structure to be filled with the next token.
 * @return Error code indicating success or failure of the operation.
 */
Error lexer_get_next_token(Token *out_token);

#endif /* IFJ26_LEXER_H */
