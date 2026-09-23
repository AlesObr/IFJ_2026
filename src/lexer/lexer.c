/**
 * @project IFJ26 Compiler
 * @file lexer.c
 * @brief Lexer module implementation for IFJ26 compiler.
 * @author Aleš Obr (xobrale00)
 * @note Standard: ISO C11
 */

#include "lexer.h"
#include "../_utils/dstring.h"
#include "../_utils/error.h"
#include "../_utils/token.h"
#include <stdio.h>
#include <stdlib.h>

/*All Global vars. belove are important to keep curent state of Lexer */
/** The current state of the lexer. */
static LexerState current_state = STATE_START;

static FILE *source_file = NULL; /** The input file being lexed - it will be de facto STDIN */
static size_t current_line = 1;	 /** The current line number in the source file being lexed */
static size_t current_col = 1;	 /** The current column number in the source file being lexed */

void lexer_init(FILE *input) {
	source_file = input;
	current_line = 1;
}

Error lexer_get_next_token(Token *out_token) {
	if (!out_token) {
		return ERROR_EMIT(ERR_INTERNAL, current_line, current_col, "Output token pointer is NULL.");
	}

	token_init(out_token);

	// @dev(Jirka Kolek) Tady budeme implementovat samotnou logiku lexeru => DKA
	out_token->type = TOKEN_EOF;
	out_token->data.str_val = NULL;
	out_token->line = current_line;
	out_token->col = current_col;

	// @dev(Jirka Kolek) Při průchodu je třeba updateovat current_line a current_col podle toho,
	// @dev kolik znaků jsme přečetli a zda jsme narazili na nový řádek.

	return ERROR_OK();
}
